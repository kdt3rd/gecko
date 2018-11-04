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
#include <map>
#include <ostream>

////////////////////////////////////////

namespace media
{

enum parameter_type
{
	PARAM_INT = 0,
	PARAM_STRING,
	PARAM_FLOAT,
	PARAM_BOOL,
	PARAM_UNKNOWN
};

parameter_type from_string( const char *pt );
const char *as_string( parameter_type pt );

class parameter_value
{
public:
	parameter_value( void );
	parameter_value( bool val );
	parameter_value( int val );
	parameter_value( int64_t val );
	parameter_value( float val );
	parameter_value( double val );
	parameter_value( base::cstring val );

	~parameter_value( void );
	parameter_value( const parameter_value &o );
	parameter_value &operator=( const parameter_value &o );
	parameter_value( parameter_value &&o );
	parameter_value &operator=( parameter_value &&o );

	bool valid( void ) const { return _ptype != PARAM_UNKNOWN; }

	inline parameter_type type( void ) const { return _ptype; }

	bool as_bool( void ) const;
	int64_t as_int( void ) const;
	double as_float( void ) const;
	const std::string &as_string( void ) const;

	void set_from_string( const std::string &v );

private:
	void destroy( void );

	parameter_type _ptype;
	union
	{
		bool _bval;
		int64_t _ival;
		double _dval;
		std::string _sval;
	};
};

std::ostream &operator<<( std::ostream &os, const parameter_value &pv );

using parameter_set = std::map<std::string, parameter_value>;

class parameter_definition
{
public:
	typedef std::pair<int64_t,int64_t> int_range;
	typedef std::pair<double,double> float_range;
	typedef std::vector<std::string> string_list;

	//parameter_definition( base::cstring name, parameter_type pt );
	parameter_definition( base::cstring name, bool defVal );
	parameter_definition( base::cstring name, int64_t minVal, int64_t maxVal, int64_t defVal );
	parameter_definition( base::cstring name, double minVal, double maxVal, double defVal );
	parameter_definition( base::cstring name, std::vector<std::string> vals, base::cstring defVal );

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

	parameter_value default_value( void ) const;

private:
	std::string _name;
	parameter_type _type;
	base::variant<int_range, float_range, string_list> _allowed_vals;
	parameter_value _def_val;
	std::string _help;
};

std::ostream &operator<<( std::ostream &os, const parameter_definition &pd );

using parameter_definitions = std::vector<parameter_definition>;

/// @brief initializes a parameter set from the definitions and the provided option
///
/// assumes any options are provided as comma separated name=value pairs. such as:
///
/// compression=jpeg,quality=95
///
parameter_set initialize_parameters(
	const parameter_definitions &params,
	const std::string &option = std::string() );

} // namespace media



