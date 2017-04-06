//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "parameter.h"
#include <base/contract.h>


////////////////////////////////////////


namespace media
{

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, parameter_type pt )
	: _name( name ), _type( pt )
{
}

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, int64_t minVal, int64_t maxVal )
		: _name( name ), _type( PARAM_INT ), _allowed_vals( std::make_pair( minVal, maxVal ) )
{
}

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, double minVal, double maxVal )
		: _name( name ), _type( PARAM_FLOAT ), _allowed_vals( std::make_pair( minVal, maxVal ) )
{
}

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, std::vector<std::string> vals )
		: _name( name ), _type( PARAM_STRING ), _allowed_vals( std::move( vals ) )
{
}

////////////////////////////////////////

const parameter_definition::int_range &
parameter_definition::range_int( void ) const
{
	precondition( _type == PARAM_INT &&
				  _allowed_vals.valid() &&
				  _allowed_vals.is<int_range>(), "querying range for integers, but not an integer type or doesn't have a range" );

	return _allowed_vals.get<int_range>();
}

////////////////////////////////////////

const parameter_definition::float_range &
parameter_definition::range_float( void ) const
{
	precondition( _type == PARAM_FLOAT &&
				  _allowed_vals.valid() &&
				  _allowed_vals.is<float_range>(), "querying range for floats, but not an float type or doesn't have a range" );

	return _allowed_vals.get<float_range>();
}

////////////////////////////////////////

const parameter_definition::string_list &
parameter_definition::string_vals( void ) const
{
	precondition( _type == PARAM_STRING &&
				  _allowed_vals.valid() &&
				  _allowed_vals.is<string_list>(), "querying allowed string values, but not a string parameter type or doesn't have a set of values" );

	return _allowed_vals.get<string_list>();
}

////////////////////////////////////////

} // media



