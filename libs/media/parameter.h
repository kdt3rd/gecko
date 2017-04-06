//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/variant.h>
#include <base/const_string.h>
#include <vector>


////////////////////////////////////////


namespace media
{

enum parameter_type
{
	PARAM_INT,
	PARAM_STRING,
	PARAM_FLOAT
};

class parameter_definition
{
public:
	typedef std::pair<int64_t,int64_t> int_range;
	typedef std::pair<double,double> float_range;
	typedef std::vector<std::string> string_list;

	parameter_definition( base::cstring name, parameter_type pt );
	parameter_definition( base::cstring name, int64_t minVal, int64_t maxVal );
	parameter_definition( base::cstring name, double minVal, double maxVal );
	parameter_definition( base::cstring name, std::vector<std::string> vals );

	parameter_definition( const parameter_definition & ) = default;
	parameter_definition( parameter_definition && ) = default;
	parameter_definition &operator=( const parameter_definition & ) = default;
	parameter_definition &operator=( parameter_definition && ) = default;
	~parameter_definition( void ) = default;

	const std::string &name( void ) const { return _name; }

	parameter_type type( void ) const { return _type; }

	void help( std::string h ) { _help = std::move( h ); }
	const std::string &help( void ) const { return _help; }

	bool has_constraint( void ) const { return _allowed_vals.valid(); }

	const int_range &range_int( void ) const;
	const std::pair<double, double> &range_float( void ) const;
	const std::vector<std::string> &string_vals( void ) const;
	
private:
	std::string _name;
	parameter_type _type;
	base::variant<int_range, float_range, string_list> _allowed_vals;
	std::string _help;
};

} // namespace media



