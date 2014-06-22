
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "utf.h"

void dump( std::u32string &str );

////////////////////////////////////////

std::map<char,char32_t> hex
{
	{ '0', 0 },
	{ '1', 1 },
	{ '2', 2 },
	{ '3', 3 },
	{ '4', 4 },
	{ '5', 5 },
	{ '6', 6 },
	{ '7', 7 },
	{ '8', 8 },
	{ '9', 9 },
	{ 'A', 10 },
	{ 'B', 11 },
	{ 'C', 12 },
	{ 'D', 13 },
	{ 'E', 14 },
	{ 'F', 15 },
};

////////////////////////////////////////

void dump( std::u32string &str )
{
	for ( size_t k = 0; k < str.size(); ++k )
		std::cout << std::hex << (uint32_t)str[k] << ' ';
	std::cout << std::dec;
}

////////////////////////////////////////

int main( void )
{
	// Normalization test.
	std::string line;
	std::ifstream file( "NormalizationTest.txt" );
	if ( !file )
	{
		std::cerr << "Error opening normalization test file" << std::endl;
		return -1;
	}
//	std::ifstream file( "test.txt" );
	int linenum = 0;
	while ( std::getline( file, line ) )
	{
		linenum++;
		if ( line.front() == '@' )
		{
			std::cout << "Section " << line.substr( 1, line.find_first_of( '#' ) ) << std::endl;
		}
		else
		{
			std::vector<std::u32string> cs;
			cs.push_back( std::u32string() );
			char32_t c = 0;
			for ( size_t i = 0; i < line.size(); ++i )
			{
				if ( line[i] == '#' )
					break;
				else if ( line[i] == ';' )
				{
					if ( c != 0 )
						cs.back().push_back( c );
					cs.push_back( std::u32string() );
					c = 0;
				}
				else if ( isspace( line[i] ) )
				{
					if ( c != 0 )
						cs.back().push_back( c );
					c = 0;
				}
				else
				{
					auto n = hex.find( line[i] );
					if ( n != hex.end() )
						c = c * 16 + n->second;
				}
			}

			cs.pop_back();
			if ( cs.size() == 5 )
			{
				bool failed = false;
				std::cout << "LINE " << linenum << std::endl;
				std::cout << "NFD ";
				dump( cs[2] );
				std::cout << " = ";
				for ( size_t i = 0; i < cs.size(); ++i )
				{
					if ( i == 3 )
					{
						std::cout << "\nNFD ";
						dump( cs[4] );
						std::cout << " = ";
					}

					std::u32string tmp;
					const std::u32string &cstr = cs[i];
					for ( size_t j = 0; j < cstr.size(); ++j )
						utf::canonical_decompose( cstr[j], tmp );
					utf::canonical_order( tmp );

					dump( tmp );
					std::cout << "; ";

					if ( i < 3 )
					{
						if ( tmp != cs[2] )
							failed = true;
					}
					else
					{
						if ( tmp != cs[4] )
							failed = true;
					}
				}
				std::cout << '\n';
				if ( failed )
					std::cout << "FAILED" << std::endl;
				failed = false;

				std::cout << "NFKD ";
				dump( cs[4] );
				std::cout << " = ";
				for ( size_t i = 0; i < cs.size(); ++i )
				{
					std::u32string tmp;
					const std::u32string &cstr = cs[i];
					for ( size_t j = 0; j < cstr.size(); ++j )
						utf::compatibility_decompose( cstr[j], tmp );
					utf::canonical_order( tmp );

					dump( tmp );
					std::cout << "; ";

					if ( tmp != cs[4] )
						failed = true;
				}
				std::cout << '\n';
				if ( failed )
					std::cout << "FAILED" << std::endl;
				failed = false;

				std::cout << "NFC ";
				dump( cs[1] );
				std::cout << " = ";
				for ( size_t i = 0; i < cs.size(); ++i )
				{
					if ( i == 3 )
					{
						std::cout << "\nNFC ";
						dump( cs[3] );
						std::cout << " = ";
					}
					std::u32string tmp;
					const std::u32string &cstr = cs[i];
					for ( size_t j = 0; j < cstr.size(); ++j )
						utf::canonical_decompose( cstr[j], tmp );
					utf::canonical_order( tmp );
					utf::canonical_compose( tmp );

					dump( tmp );
					std::cout << "; ";

					if ( i < 3 )
					{
						if ( tmp != cs[1] )
							failed = true;
					}
					else
					{
						if ( tmp != cs[3] )
							failed = true;
					}
				}
				std::cout << '\n';

				std::cout << "NFKC ";
				dump( cs[3] );
				std::cout << " = ";
				for ( size_t i = 0; i < cs.size(); ++i )
				{
					std::u32string tmp;
					const std::u32string &cstr = cs[i];
					for ( size_t j = 0; j < cstr.size(); ++j )
						utf::compatibility_decompose( cstr[j], tmp );
					utf::canonical_order( tmp );
					utf::canonical_compose( tmp );

					dump( tmp );
					std::cout << "; ";

					if ( tmp != cs[3] )
						failed = true;
				}
				std::cout << '\n';

				if ( failed )
					std::cout << "FAILED" << std::endl;
				failed = false;
			}
		}
	}

	return 0;
}

////////////////////////////////////////

