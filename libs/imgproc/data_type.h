//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/contract.h>
#include <utf/utf.h>

namespace imgproc
{

////////////////////////////////////////

enum class pod_type
{
	BOOLEAN,
	INT8,
	UINT8,
	INT16,
	UINT16,
	INT32,
	UINT32,
	INT64,
	UINT64,
	FLOAT16,
	FLOAT32,
	FLOAT64,
	UNKNOWN,
};

////////////////////////////////////////

inline bool is_signed( pod_type t )
{
	switch ( t )
	{
		case pod_type::INT8:
		case pod_type::INT16:
		case pod_type::INT32:
		case pod_type::INT64:
			return true;

		case pod_type::BOOLEAN:
		case pod_type::UINT8:
		case pod_type::UINT16:
		case pod_type::UINT32:
		case pod_type::UINT64:
		case pod_type::FLOAT16:
		case pod_type::FLOAT32:
		case pod_type::FLOAT64:
		case pod_type::UNKNOWN:
			break;
	}
	return false;
}

////////////////////////////////////////

inline bool is_unsigned( pod_type t )
{
	switch ( t )
	{
		case pod_type::UINT8:
		case pod_type::UINT16:
		case pod_type::UINT32:
		case pod_type::UINT64:
			return true;

		case pod_type::BOOLEAN:
		case pod_type::INT8:
		case pod_type::INT16:
		case pod_type::INT32:
		case pod_type::INT64:
		case pod_type::FLOAT16:
		case pod_type::FLOAT32:
		case pod_type::FLOAT64:
		case pod_type::UNKNOWN:
			break;
	}

	return false;
}

////////////////////////////////////////

class data_type
{
public:
	data_type( void )
		: _type( pod_type::UNKNOWN ), _dims( 0 )
	{
	}

	data_type( pod_type t, size_t d )
		: _type( t ), _dims( d )
	{
	}

	pod_type base_type( void ) const { return _type; }
	size_t dimensions( void ) const { return _dims; }

	bool operator==( const data_type &o ) const
	{
		return _type == o._type && _dims == o._dims;
	}

	bool operator!=( const data_type &o ) const
	{
		return _type != o._type || _dims != o._dims;
	}

	bool operator<( const data_type &o ) const
	{
		if ( _type < o._type )
			return true;
		else if ( _type == o._type )
			return _dims < o._dims;
		return false;
	}

	void set_type( pod_type t )
	{
		_type = t;
	}

	void set_dimensions( size_t d )
	{
		_dims = d;
	}

private:
	pod_type _type;
	size_t _dims;
};

////////////////////////////////////////

inline pod_type merge( pod_type t1, pod_type t2 )
{
	// If both type match, all done
	if ( t1 == t2 )
		return t1;

	// Return the "greater" type
	if ( t1 < t2 )
		return t2;

	return t1;
}

////////////////////////////////////////

inline std::string type_name( pod_type t )
{
	switch ( t )
	{
		case pod_type::BOOLEAN: return "bool";
		case pod_type::UINT8: return "uint8";
		case pod_type::UINT16: return "uint16";
		case pod_type::UINT32: return "uint32";
		case pod_type::UINT64: return "uint64";
		case pod_type::INT8: return "int8";
		case pod_type::INT16: return "int16";
		case pod_type::INT32: return "int32";
		case pod_type::INT64: return "int64";
		case pod_type::FLOAT16: return "half";
		case pod_type::FLOAT32: return "float";
		case pod_type::FLOAT64: return "double";
		case pod_type::UNKNOWN: return "unknown";
	}
	return std::string();
}

////////////////////////////////////////

inline pod_type type_enum( const std::u32string &t )
{
	if ( t == U"bool" ) return pod_type::BOOLEAN;
	if ( t == U"uint8" ) return pod_type::UINT8;
	if ( t == U"uint16" ) return pod_type::UINT16;
	if ( t == U"uint32" ) return pod_type::UINT32;
	if ( t == U"uint64" ) return pod_type::UINT64;
	if ( t == U"int8" ) return pod_type::INT8;
	if ( t == U"int16" ) return pod_type::INT16;
	if ( t == U"int32" ) return pod_type::INT32;
	if ( t == U"int64" ) return pod_type::INT64;
	if ( t == U"half" ) return pod_type::FLOAT16;
	if ( t == U"float" ) return pod_type::FLOAT32;
	if ( t == U"double" ) return pod_type::FLOAT64;
	if ( t == U"unknown" ) return pod_type::UNKNOWN;
	return pod_type::UNKNOWN;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, pod_type t )
{
	out << type_name( t );
	return out;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const data_type &t )
{
	out << type_name( t.base_type() );
	if ( t.dimensions() > 0 )
		out << '*' << t.dimensions();
	return out;
}

////////////////////////////////////////

inline std::string cpp_type( const data_type &t )
{
	std::ostringstream tmp;
	if ( t.dimensions() > 0 )
		tmp << "buffer<" << t << '>';
	else
		tmp << t.base_type();
	return tmp.str();
}

////////////////////////////////////////

inline std::string cpp_type_const_ref( const data_type &t, const std::u32string &mod )
{
	std::ostringstream tmp;
	if ( t.dimensions() > 0 )
	{
		if ( mod.empty() )
			tmp << "const buffer<" << t << "> &";
		else
			tmp << "const buffer_" << mod << "<" << t << "> &";
	}
	else
		tmp << t.base_type() << ' ';
	return tmp.str();
}

////////////////////////////////////////

}

