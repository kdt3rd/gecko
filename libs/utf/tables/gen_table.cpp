//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iomanip>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <zlib.h>

#include "compressed_table.h"
#include "xml.h"

// All of the data from the Unicode database
constexpr char32_t maxcp = 0x110000;
static std::vector<std::string> names( maxcp ) ;
static std::vector<uint8_t> category( maxcp, 0 );
static std::vector<uint8_t> combining( maxcp, 0 );
static std::vector<uint32_t> property( maxcp, 0 );
static std::vector<uint16_t> decomptable( maxcp, 0 );
static std::vector<uint8_t> decomptype( maxcp, 0 );
static std::vector<uint8_t> compexcl( maxcp, 0 );
static std::vector<double> nvalue( maxcp, std::numeric_limits<double>::quiet_NaN() );
static std::u32string decompose{ 0 };
static std::vector<uint16_t> compbase( maxcp, 0xFFFF );
static std::vector<uint8_t> compcomb( maxcp, 0xFF );

static std::vector<std::tuple<std::string,char32_t,char32_t>> blocks;

////////////////////////////////////////

namespace
{
	template<typename T>
	T from_string( const std::string &str, bool hex = false )
	{
		T t;
		std::stringstream tmp( str );
		if ( hex )
			tmp >> std::hex;
		tmp >> t;
		return t;
	}
}

////////////////////////////////////////

static std::map<std::string,uint32_t> propNames
{
	{ "XIDS",		1<<0 },
	{ "XIDC",		1<<1 },
	{ "Pat_WS",		1<<2 },
	{ "Pat_Syn",	1<<3 },
	{ "Math",		1<<4 },
	{ "WSpace",		1<<5 },
	{ "Hex",		1<<6 },
	{ "Dash",		1<<8 },
	{ "Hyphen",		1<<9 },
	{ "QMark",		1<<10 },
	{ "Term",		1<<11 },
	{ "Upper",		1<<12 },
	{ "Lower",		1<<13 },
	{ "Alpha",		1<<14 },
	{ "OUpper",		1<<15 },
	{ "OLower",		1<<16 },
	{ "OAlpha",		1<<17 },
	{ "OMath",		1<<18 },
	{ "NChar",		1<<19 },
	{ "Nu",			1<<20 },
	{ "De",			1<<21 },
	{ "Di",			1<<22 },
};

////////////////////////////////////////

static std::map<std::string,uint8_t> catNames
{
	{ "Cc", 1 },
	{ "Cf", 2 },
	{ "Cn", 3 },
	{ "Co", 4 },
	{ "Cs", 5 },
	{ "Ll", 6 },
	{ "Lm", 7 },
	{ "Lo", 8 },
	{ "Lt", 9 },
	{ "Lu", 10 },
	{ "Mc", 11 },
	{ "Me", 12 },
	{ "Mn", 13 },
	{ "Nd", 14 },
	{ "Nl", 15 },
	{ "No", 16 },
	{ "Pc", 17 },
	{ "Pd", 18 },
	{ "Pe", 19 },
	{ "Pf", 20 },
	{ "Pi", 21 },
	{ "Po", 22 },
	{ "Ps", 23 },
	{ "Sc", 24 },
	{ "Sk", 25 },
	{ "Sm", 26 },
	{ "So", 27 },
	{ "Zl", 28 },
	{ "Zp", 29 },
	{ "Zs", 30 }
};

////////////////////////////////////////

static std::map<std::string,double> numberValues
{
	{ "-1/2", -1.0/2.0 },
	{ "1/10", 1.0/10.0 },
	{ "1/16", 1.0/16.0 },
	{ "1/2", 1.0/2.0 },
	{ "1/3", 1.0/3.0 },
	{ "1/4", 1.0/4.0 },
	{ "1/5", 1.0/5.0 },
	{ "1/6", 1.0/6.0 },
	{ "1/7", 1.0/7.0 },
	{ "1/8", 1.0/8.0 },
	{ "1/9", 1.0/9.0 },
	{ "11/2", 11.0/2.0 },
	{ "13/2", 13.0/2.0 },
	{ "15/2", 15.0/2.0 },
	{ "17/2", 17.0/2.0 },
	{ "2/3", 2.0/3.0 },
	{ "2/5", 2.0/5.0 },
	{ "3/16", 3.0/16.0 },
	{ "3/2", 3.0/2.0 },
	{ "3/4", 3.0/4.0 },
	{ "3/5", 3.0/5.0 },
	{ "3/8", 3.0/8.0 },
	{ "4/5", 4.0/5.0 },
	{ "5/2", 5.0/2.0 },
	{ "5/6", 5.0/6.0 },
	{ "5/8", 5.0/8.0 },
	{ "7/2", 7.0/2.0 },
	{ "7/8", 7.0/8.0 },
	{ "9/2", 9.0/2.0 }
};

////////////////////////////////////////

static std::map<std::string,uint8_t> decompNames
{
	{ "none", 0 },
	{ "can", 1 },
	{ "com", 2 },
	{ "sup", 3 },
	{ "fra", 4 },
	{ "nb", 5 },
	{ "sub", 6 },
	{ "font", 7 },
	{ "enc", 8 },
	{ "wide", 9 },
	{ "vert", 10 },
	{ "sqr", 11 },
	{ "iso", 12 },
	{ "fin", 13 },
	{ "init", 14 },
	{ "med", 15 },
	{ "sml", 16 },
	{ "nar", 17 }
};

////////////////////////////////////////

class ucd_reader : public xml::reader
{
public:
	virtual ~ucd_reader( void );

	virtual void begin_element( const std::string &name )
	{
		first = last = 0xFFFFFFFF;
		if ( name == "char" )
			mode.push_back( CHARACTER );
		else if ( name == "block" )
			mode.push_back( BLOCK );
		else if ( name == "surrogate" )
			mode.push_back( SURROGATE );
		else if ( name == "non-character" )
			mode.push_back( NONCHAR );
		else if ( name == "reserved" )
			mode.push_back( RESERVED );
		else
			mode.push_back( UNKNOWN );
	}

	virtual void content( const std::string & )
	{
	}

	virtual void attribute( const std::string &name, const std::string &value )
	{
		assert( !mode.empty() );
		if ( mode.back() == CHARACTER || mode.back() == SURROGATE || mode.back() == RESERVED || mode.back() == NONCHAR )
		{
			if ( name == "cp" )
			{
				first = last = from_string<uint32_t>( "0x" + value, true );
			}
			else if ( name == "first-cp" )
			{
				first = from_string<uint32_t>( "0x" + value, true );
			}
			else if ( name == "last-cp" )
			{
				last = from_string<uint32_t>( "0x" + value, true );
			}
			else if ( first < 0x110000 && last < 0x110000 )
			{
				if ( name == "na" )
				{
					for ( char32_t cp = first; cp <= last; ++cp )
						names[cp] = value;
				}
				else if ( name == "gc" )
				{
					for ( char32_t cp = first; cp <= last; ++cp )
						category[cp] = catNames[value];
				}
				else if ( name == "ccc" )
				{
					uint8_t v = uint8_t( from_string<uint32_t>( value ) );
					for ( char32_t cp = first; cp <= last; ++cp )
						combining[cp] = v;
				}
				else if ( name == "dm" )
				{
					if ( value != "#" )
					{
						std::stringstream tmp( value );
						tmp >> std::hex;
						std::u32string decomp;
						uint32_t v;
						while( tmp >> v )
							decomp.push_back( char32_t(v) );
						if( decomp.empty() )
							throw std::runtime_error( "Didn't read any decomposition characters" );
						decomp.push_back( 0 );

						size_t decompoff = decompose.find( decomp );
						if ( decompoff == std::u32string::npos )
						{
							decompoff = decompose.size();
							decompose.append( decomp );
						}

						for ( uint32_t cp = first; cp <= last; ++cp )
							decomptable[cp] = uint16_t( decompoff );
					}
				}
				else if ( name == "dt" )
				{
					auto dt = decompNames.find( value );
					if( dt == decompNames.end() )
					{
						std::cerr << "Decomp type = " << value <<std::endl;
						throw std::runtime_error( "decomp type not found" );
					}
					for ( uint32_t cp = first; cp <= last; ++cp )
						decomptype[cp] = dt->second;
				}
				else if ( name == "Comp_Ex" )
				{
					if ( value == "Y" )
					{
						for ( uint32_t cp = first; cp <= last; ++cp )
							compexcl[cp] = 1;
					}
				}
				else if ( name == "nt" )
				{
					auto p = propNames.find( value );
					if ( p != propNames.end() )
					{
						uint32_t flag = p->second;
						for ( char32_t cp = first; cp <= last; ++cp )
							property[cp] |= flag;
					}
				}
				else if ( name == "nv" && value != "NaN" )
				{
					double v = 0.0;
					auto i = numberValues.find( value );
					if ( i != numberValues.end() )
						v = i->second;
					else
					{
						size_t pos = 0;
						v = stod( value, &pos );
						if ( pos != value.size() )
						{
							std::cout << pos << " = " << value << std::endl;
							throw std::runtime_error( "Couldn't parse value" );
						}
					}
					for ( char32_t cp = first; cp <= last; ++cp )
						nvalue[cp] = v;

					/*
					static std::set<std::string> nvals;
					if ( nvals.find( value ) == nvals.end() )
					{
						std::cout << "Number type = " << value << std::endl;
						nvals.insert( value );
					}
					*/
				}
				else if ( value == "Y" )
				{
					auto p = propNames.find( name );
					if ( p != propNames.end() )
					{
						uint32_t flag = p->second;
						for ( char32_t cp = first; cp <= last; ++cp )
							property[cp] |= flag;
					}
				}
			}
		}
		else if ( mode.back() == BLOCK )
		{
			if ( name == "first-cp" )
			{
				first = from_string<uint32_t>( "0x" + value, true );
			}
			else if ( name == "last-cp" )
			{
				last = from_string<uint32_t>( "0x" + value, true );
			}
			else if ( name == "name" )
			{
				blocks.push_back( std::make_tuple( name, first, last ) );
			}
		}
	}

	virtual void end_element( const std::string & )
	{
		mode.pop_back();
		first = last = 0xFFFFFFFF;
	}

private:
	enum Mode
	{
		UNKNOWN,
		CHARACTER,
		SURROGATE,
		RESERVED,
		NONCHAR,
		BLOCK
	};
	std::vector<Mode> mode;
	char32_t first, last;
};

ucd_reader::~ucd_reader( void )
{
}

////////////////////////////////////////

static std::map<std::string,std::string> catTypes
{
	{ "Cc", "other_control" },
	{ "Cf", "other_format" },
	{ "Cn", "other_not_assigned" },
	{ "Co", "other_private" },
	{ "Cs", "other_surrogate" },
	{ "Ll", "letter_lowercase" },
	{ "Lm", "letter_modifier" },
	{ "Lo", "letter_other" },
	{ "Lt", "letter_titlecase" },
	{ "Lu", "letter_uppercase" },
	{ "Mc", "mark_combining" },
	{ "Me", "mark_enclosing" },
	{ "Mn", "mark_nonspacing" },
	{ "Nd", "number_decimal" },
	{ "Nl", "number_letter" },
	{ "No", "number_other" },
	{ "Pc", "punctuation_connector" },
	{ "Pd", "punctuation_dash" },
	{ "Pe", "punctuation_close" },
	{ "Pf", "punctuation_final" },
	{ "Pi", "punctuation_initial" },
	{ "Po", "punctuation_other" },
	{ "Ps", "punctuation_open" },
	{ "Sc", "symbol_currency" },
	{ "Sk", "symbol_modifier" },
	{ "Sm", "symbol_math" },
	{ "So", "symbol_other" },
	{ "Zl", "separator_line" },
	{ "Zp", "separator_paragraph" },
	{ "Zs", "separator_space" }
};

static std::map<std::string,std::string> catMainTypes
{
	{ "C", "other" },
	{ "L", "letter" },
	{ "M", "mark" },
	{ "N", "number" },
	{ "P", "punctuation" },
	{ "S", "symbol" },
	{ "Z", "separator" },
};

static std::map<std::string,std::string> propTypes
{
	{ "XIDS",		"identifier_start" },
	{ "XIDC",		"identifier_continue" },
	{ "Pat_WS",		"pattern_whitespace" },
	{ "Pat_Syn",	"pattern_syntax" },
	{ "Math",		"math" },
	{ "WSpace",		"whitespace" },
	{ "Hex",		"hex_digit" },
	{ "Dash",		"dash" },
	{ "Hyphen",		"hyphen" },
	{ "QMark",		"question_mark" },
	{ "Term",		"terminal_punctuation" },
	{ "Upper",		"uppercase" },
	{ "Lower",		"lowercase" },
	{ "Alpha",		"alphabetic" },
	{ "OUpper",		"other_uppercase" },
	{ "OLower",		"other_lowercase" },
	{ "OAlpha",		"other_alphabetic" },
	{ "OMath",		"other_math" },
	{ "NChar",		"non_character" },
	{ "Nu",			"numeric" },
	{ "De",			"decimal" },
	{ "Di",			"digit" },
};

static std::map<std::string,uint8_t> bidiNames =
{
	{ "L", 1 },
	{ "LRE", 2 },
	{ "LRO", 3 },
	{ "R", 4 },
	{ "AL", 5 },
	{ "RLE", 6 },
	{ "RLO", 7 },
	{ "PDF", 8 },
	{ "EN", 9 },
	{ "ES", 10 },
	{ "ET", 11 },
	{ "AN", 12 },
	{ "CS", 13 },
	{ "NSM", 14 },
	{ "BN", 15 },
	{ "B", 16 },
	{ "S", 17 },
	{ "WS", 18 },
	{ "ON", 19 },
};

////////////////////////////////////////

int safemain( int argc, char *argv[] );

int safemain( int argc, char *argv[] )
{
	if ( argc != 2 )
	{
		std::cerr << "Usage: " << argv[0] << " <xml_ucd_database>" << std::endl;
		return -1;
	}

	{
		std::string fn = argv[1];
		ucd_reader reader;

		std::ifstream in( fn.c_str() );
		if ( fn.find( ".gz" ) != std::string::npos )
		{
			static const size_t kChunk = 16384;
			z_stream strm;
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;

			Bytef tmpBuf[kChunk];

			std::string slurp;
			while ( in )
			{
				ssize_t n = in.read( reinterpret_cast<char *>( tmpBuf ), kChunk ).gcount();
				if ( n > 0 )
					slurp.append( reinterpret_cast<char *>( tmpBuf ), static_cast<size_t>( n ) );
			}

			strm.avail_in = static_cast<uInt>( slurp.size() );
			strm.next_in = reinterpret_cast<Bytef *>( const_cast<char *>( slurp.data() ) );

			int zErr = inflateInit2( &strm, 16+MAX_WBITS );
			if ( zErr != Z_OK )
			{
				std::cerr << "Unable to init compression library" << std::endl;
				return -1;
			}

			std::string decomp;

			while ( zErr != Z_STREAM_END )
			{
				strm.avail_out = kChunk;
				strm.next_out = tmpBuf;

				zErr = inflate( &strm, Z_SYNC_FLUSH );
				if ( zErr != Z_OK && zErr != Z_STREAM_END )
				{
					std::cerr << "Error reading compressed file: " << zErr << std::endl;
					return -1;
				}

				decomp.append( reinterpret_cast<char *>( tmpBuf ), kChunk - strm.avail_out );
			}

			zErr = inflateEnd( &strm );

			std::stringstream x( decomp );
			x >> reader;
		}
		else
		{
			in >> reader;
		}
	}

	// Create composition information
	std::vector<std::vector<char32_t>> comp_chart;
	uint8_t maxcomb = 0;
	for ( char32_t cp = 0; cp < maxcp; ++cp )
	{
		if ( compexcl[cp] == 0 && decomptype[cp] == 1 )
		{
			// Check the decomposition...
			uint16_t off = decomptable[cp];
			char32_t base = decompose[off];
			if ( decompose[off+1] == 0 )
				throw std::runtime_error( "Singleton decomposition should not be composed" );
			char32_t comb = decompose[off+1];
			if ( decompose[off+2] != 0 )
				throw std::runtime_error( "More than 2 character in composition" );

			uint16_t baseoff = compbase[base];
			if ( baseoff == 0xFFFF )
			{
				baseoff = uint16_t( comp_chart.size() );
				compbase[base] = baseoff;
				comp_chart.push_back( std::vector<char32_t>() );
			}

			std::vector<char32_t> &comb_chart = comp_chart[baseoff];

			uint8_t comboff = compcomb[comb];
			if ( comboff == 0xFF )
			{
				comboff = maxcomb++;
				compcomb[comb] = comboff;
			}
			comb_chart.resize( std::max( uint32_t(comboff+1), uint32_t(comb_chart.size()) ), 0 );
			comb_chart[comboff] = cp;
		}
	}
	for ( size_t i = 0; i < comp_chart.size(); ++i )
		comp_chart[i].resize( maxcomb, 0x0 );

	// Add in hexadecimal digits
	for ( int64_t i = 0; i < 6; ++i )
	{
		nvalue[U'a' + char32_t(i)] = 10.0 + i;
		nvalue[U'A' + char32_t(i)] = 10.0 + i;
		nvalue[U'\uff21' + char32_t(i)] = 10.0 + i;
		nvalue[U'\uff41' + char32_t(i)] = 10.0 + i;
	}

	// Create arrays of numerical values
	std::vector<uint8_t> valueidx( maxcp, 0 );
	std::vector<double> doubleVals( 1, std::numeric_limits<double>::quiet_NaN() );
	std::vector<uint64_t> intVals( 1, std::numeric_limits<uint64_t>::max() );
	for ( size_t i = 0; i < maxcp; ++i )
	{
		if ( !std::isnan( nvalue[i] ) )
		{
			double v = nvalue[i];
			size_t f = 0;
			for ( f = 0; f < doubleVals.size(); ++f )
			{
				if ( Equal<double>::test( v, doubleVals[f] ) )
					break;
			}
			if ( f == doubleVals.size() )
			{
				doubleVals.push_back( v );

				if ( v >= std::numeric_limits<uint64_t>::lowest() && v < std::numeric_limits<uint64_t>::max() )
				{
					uint64_t iv = static_cast<uint64_t>( v );
					if ( Equal<double>::test( v, double(iv) ) )
						intVals.push_back( iv );
					else
						intVals.push_back( std::numeric_limits<uint64_t>::max() );
				}
				else
					intVals.push_back( std::numeric_limits<uint64_t>::max() );
			}

			valueidx[i] = static_cast<uint8_t>( f );
		}
	}
	if ( doubleVals.size() > 254 )
		throw std::runtime_error( "Number array overflow" );

	{
		compressed_table<uint8_t> category_table( category, 256 );

		std::ofstream out( "utfcat.cpp" );
		out <<
			"// Automatically generated file\n"
			"\n"
			"#include <utfcat.h>\n"
			"#include <cstdint>\n"
			"\n"
			"////////////////////////////////////////\n"
			"\n";

		out << "// Private namespace\n";
		out << "namespace {\n";
		out << "// Category tables\n";
		category_table.save( out, "uint8_t", "category" );
		out << "} // end of private namespace\n";

		out << "\n";
		out << "namespace utf\n";
		out << "{\n";
		for ( auto i = catTypes.begin(); i != catTypes.end(); ++i )
		{
			out << "bool is_" << i->second << "( char32_t cp )\n";
			out << "{\n";
			out << "\treturn get_category( cp ) == " << static_cast<uint32_t>( catNames[i->first] ) << ";\n";
			out << "}\n";
		}
		for ( auto i = catMainTypes.begin(); i != catMainTypes.end(); ++i )
		{
			out << "bool is_" << i->second << "( char32_t cp )\n";
			out << "{\n";
			uint8_t tmax = 0;
			uint8_t tmin = 0xFF;
			for ( auto j = catNames.begin(); j != catNames.end(); ++j )
			{
				if ( i->first[0] == j->first[0] )
				{
					if ( j->second > tmax )
						tmax = j->second;
					if ( j->second < tmin )
						tmin = j->second;
				}
			}
			tmin--;
			tmax++;
			out << "\tuint8_t cat = get_category( cp );\n";
			out << "\treturn cat > " << static_cast<uint32_t>( tmin ) << " && cat < " << static_cast<uint32_t>( tmax ) << ";\n";
			out << "}\n";
		}
		out << "bool is_unknown( char32_t cp )\n";
		out << "{\n";
		out << "\treturn get_category( cp ) == 0;\n";
		out << "}\n";

		out << "bool is_graphic( char32_t cp )\n";
		out << "{\n";
		out << "\tuint8_t cat = get_category( cp );\n";
		out << "\treturn ( cat > 5 && cat < 28 ) || cat == 30;\n";
		out << "}\n";

		out << "bool is_format( char32_t cp )\n";
		out << "{\n";
		out << "\tuint8_t cat = get_category( cp );\n";
		out << "\treturn cat == 2 || cat == 28 || cat == 29;\n";
		out << "}\n";

		out << "bool is_control( char32_t cp )\n";
		out << "{\n";
		out << "\treturn get_category( cp ) == 1;\n";
		out << "}\n";

		out << "bool is_private( char32_t cp )\n";
		out << "{\n";
		out << "\treturn get_category( cp ) == 4;\n";
		out << "}\n";

		out << "bool is_surrogate( char32_t cp )\n";
		out << "{\n";
		out << "\treturn get_category( cp ) == 5;\n";
		out << "}\n";

		out << "bool is_reserved( char32_t cp )\n";
		out << "{\n";
		out << "\treturn get_category( cp ) == 3;\n";
		out << "}\n";

		out << "}\n";
	}

	{
		compressed_table<uint32_t> property_table( property, 128 );
		compressed_table<uint8_t> value_table( valueidx, 128 );

		std::ofstream out( "utfprop.cpp" );
		out <<
			"// Automatically generated file\n"
			"\n"
			"#include <utfprop.h>\n"
			"#include <cstdint>\n"
			"#include <limits>\n"
			"\n"
			"////////////////////////////////////////\n"
			"\n";

		out << "\n";
		out << "// Private namespace\n";
		out << "namespace {\n";
		out << "// Property tables\n";
		property_table.save( out, "uint32_t", "property" );
		out << "// Value tables\n";
		value_table.save( out, "uint8_t", "value" );
		out << "} // end of private namespace\n";

		out << "\n";
		out << "namespace utf\n";
		out << "{\n";
		for ( auto i = propTypes.begin(); i != propTypes.end(); ++i )
		{
			out << "bool is_" << i->second << "( char32_t cp )\n";
			out << "{\n";
			out << "\treturn ( get_property( cp ) & " << static_cast<uint32_t>( propNames[i->first] ) << " ) != 0;\n";
			out << "}\n";
		}

		out << "double number_value( char32_t cp )\n";
		out << "{\n";
		out << "\tstatic double dvalues[] = { std::numeric_limits<double>::quiet_NaN()";
		for ( size_t i = 1; i < doubleVals.size(); ++i )
			out << ", " << doubleVals[i];
		out << "};\n\n";
		out << "\treturn dvalues[ get_value( cp ) ];\n";
		out << "}\n";

		out << "uint64_t integer_value( char32_t cp )\n";
		out << "{\n";
		out << "\tstatic uint64_t ivalues[] = { " << std::numeric_limits<uint64_t>::max() << 'U';
		for ( size_t i = 1; i < intVals.size(); ++i )
			out << ", " << intVals[i] << 'U';
		out << "};\n\n";
		out << "\treturn ivalues[ get_value( cp ) ];\n";
		out << "}\n";

		out << "}\n";
	}

	{
		compressed_table<uint8_t> combining_table( combining, 512 );
		compressed_table<uint16_t> decompose_table( decomptable, 512 );
		compressed_table<uint8_t> decomptype_table( decomptype, 512 );
		compressed_table<uint16_t> compbase_table( compbase, 512 );
		compressed_table<uint8_t> compcomb_table( compcomb, 512 );

		std::ofstream out( "utfnorm.cpp" );
		out <<
			"// Automatically generated file\n"
			"\n"
			"#include <utfnorm.h>\n"
			"#include <cstdint>\n"
			"\n"
			"////////////////////////////////////////\n"
			"\n";

		out << "\n";
		out << "// Private namespace\n";
		out << "namespace {\n";

		out << "// Combining class tables\n";
		combining_table.save( out, "uint8_t", "combining" );

		out << "// Decompose character string\n";
		out << "char32_t decomp_char[" << decompose.size() << "] =\n";
		out << "{";
		out << std::hex;
		for ( size_t i = 0; i < decompose.size(); ++i )
		{
			if ( i % 16 == 0 )
				out << "\n\t";
			out << "0x" << uint32_t(decompose[i]);
			if ( i < decompose.size()-1 )
				out << ",";
		}
		out << std::dec;
		out << "\n};\n";

		out << "// Decompose type\n";
		decomptype_table.save( out, "uint8_t", "decomptype" );

		out << "// Decompose offset tables\n";
		decompose_table.save( out, "uint16_t", "decompose" );

		out << "// Compose base\n";
		compbase_table.save( out, "uint16_t", "compbase" );

		out << "// Compose combine\n";
		compcomb_table.save( out, "uint8_t", "compcomb" );

		out << "// Compose character string\n";
		out << "char32_t comp_char[" << comp_chart.size() * maxcomb << "] =\n";
		out << "{";
		out << std::hex;
		for ( size_t i = 0; i < comp_chart.size(); ++i )
		{
			size_t N = comp_chart[i].size();
			out << "\n\t";
			for ( size_t j = 0; j < N; ++j )
			{
				out << "0x" << uint32_t(comp_chart[i][j]);
				if ( j < N-1 || i < comp_chart.size()-1 )
					out << ",";
			}
		}
		out << std::dec;
		out << "\n};\n";

		out << "} // end of private namespace\n";

		out << "\n";
		out << "namespace utf\n";
		out << "{\n";

		out << "uint8_t canonical_combining_class( char32_t cp )\n";
		out << "{\n";
		out << "\treturn get_combining( cp );\n";
		out << "}\n";

		out << "void canonical_decompose( char32_t cp, std::u32string &str )\n";
		out << "{\n";
		out << "\tif( get_decomptype( cp ) == 1 )\n";
		out << "\t{\n";
		out << "\t\tuint16_t off = get_decompose( cp );\n";
		out << "\t\tif ( off > 0 )\n";
		out << "\t\t{\n";
		out << "\t\t\twhile( decomp_char[off] )\n";
		out << "\t\t\t\tcanonical_decompose( decomp_char[off++], str );\n";
		out << "\t\t}\n";
		out << "\t\telse\n";
		out << "\t\t\tstr.push_back( cp );\n";
		out << "\t}\n";
		out << "\telse\n";
		out << "\t\tstr.push_back( cp );\n";
		out << "}\n";

		out << "void compatibility_decompose( char32_t cp, std::u32string &str )\n";
		out << "{\n";
		out << "\tuint16_t off = get_decompose( cp );\n";
		out << "\tif ( off > 0 )\n";
		out << "\t{\n";
		out << "\t\twhile( decomp_char[off] )\n";
		out << "\t\t\tcompatibility_decompose( decomp_char[off++], str );\n";
		out << "\t}\n";
		out << "\telse\n";
		out << "\t\tstr.push_back( cp );\n";
		out << "}\n";

		out << "uint16_t compose_base( char32_t base )\n";
		out << "{\n";
		out << "\treturn get_compbase( base );\n";
		out << "}\n";
		out << "uint8_t compose_combine( char32_t comb )\n";
		out << "{\n";
		out << "\treturn get_compcomb( comb );\n";
		out << "}\n";
		out << "char32_t compose_char( uint32_t b, uint32_t c )\n";
		out << "{\n";
		out << "\tif ( b < 0xFFFF && c < 0xFF )\n";
		out << "\t\treturn comp_char[b*" << int(maxcomb) << "+c];\n";
		out << "\treturn 0;\n";
		out << "}\n";

		out << "}\n";
	}

	/*
	for( size_t bs = 8; bs <= 4096; bs *= 2 )
	{
		std::cout << "Compressing at " << bs << std::endl;
		compressed_table<uint8_t> table( valueidx, bs );
		size_t mem = table.data_size() * sizeof(uint16_t) + table.num_blocks()*2;
		std::cerr << "Table @" << bs << " " << mem << std::endl;
	}
	*/

	return 0;
}

////////////////////////////////////////

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( std::exception &e )
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return -1;
}

////////////////////////////////////////

