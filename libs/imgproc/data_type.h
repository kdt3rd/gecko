
#pragma once

#include <base/contract.h>

namespace imgproc
{

////////////////////////////////////////

enum class data_type
{
	UINT8,
	INT8,
	UINT16,
	INT16,
	UINT32,
	INT32,
	UINT64,
	INT64,
	FLOAT32,
	FLOAT64,
	UNKNOWN,
};

////////////////////////////////////////

typedef std::pair<data_type,size_t> type;

////////////////////////////////////////

inline std::string type_name( data_type t )
{
	switch ( t )
	{
		case data_type::UINT8: return "uint8_t";
		case data_type::UINT16: return "uint16_t";
		case data_type::UINT32: return "uint32_t";
		case data_type::UINT64: return "uint64_t";
		case data_type::INT8: return "int8_t";
		case data_type::INT16: return "int16_t";
		case data_type::INT32: return "int32_t";
		case data_type::INT64: return "int64_t";
		case data_type::FLOAT32: return "float";
		case data_type::FLOAT64: return "double";
		default: throw_logic( "unknown data type" );
	}
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, data_type t )
{
	out << type_name( t );
	return out;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const type &t )
{
	out << type_name( t.first ) << ',' << t.second;
	return out;
}

////////////////////////////////////////

inline std::string cpp_type( const type &t )
{
	std::ostringstream tmp;
	if ( t.second > 0 )
		tmp << "buffer<" << t << '>';
	else
		tmp << t.first;
	return tmp.str();
}

////////////////////////////////////////

inline std::string cpp_type_const_ref( const type &t )
{
	std::ostringstream tmp;
	if ( t.second > 0 )
		tmp << "const buffer<" << t << "> &";
	else
		tmp << t.first << ' ';
	return tmp.str();
}

////////////////////////////////////////

}

