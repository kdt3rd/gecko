//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "parameter.h"
#include <base/contract.h>
#include <base/string_util.h>

////////////////////////////////////////

namespace
{

static const char *ptype_names[] = 
{
	"int",
	"string",
	"number",
	"bool"
};
static constexpr size_t k_ptype_count = sizeof(ptype_names)/sizeof(const char *);

} // empty namespace

namespace media
{

////////////////////////////////////////

parameter_type from_string( const char *pt )
{
	if ( pt )
	{
		for ( size_t p = 0; p < k_ptype_count; ++p )
		{
			if ( ! strcmp( pt, ptype_names[p] ) )
				return static_cast<parameter_type>( p );
		}
	}
	return PARAM_UNKNOWN;
}

const char *as_string( parameter_type pt )
{
	if ( pt < PARAM_UNKNOWN )
		return ptype_names[pt];
	return "unknown";
}

////////////////////////////////////////

parameter_value::parameter_value( void )
	: _ptype( PARAM_UNKNOWN )
{
}

////////////////////////////////////////

parameter_value::parameter_value( bool val )
	: _ptype( PARAM_BOOL ), _bval( val )
{
}

////////////////////////////////////////

parameter_value::parameter_value( int val )
	: _ptype( PARAM_INT ), _ival( val )
{
}

////////////////////////////////////////

parameter_value::parameter_value( int64_t val )
	: _ptype( PARAM_INT ), _ival( val )
{
}

////////////////////////////////////////

parameter_value::parameter_value( float val )
	: _ptype( PARAM_FLOAT ), _dval( val )
{
}

////////////////////////////////////////

parameter_value::parameter_value( double val )
	: _ptype( PARAM_FLOAT ), _dval( val )
{
}

////////////////////////////////////////

parameter_value::parameter_value( base::cstring val )
	: _ptype( PARAM_STRING )
{
	new (&_sval) std::string( val );
}

////////////////////////////////////////

parameter_value::~parameter_value( void )
{
	destroy();
}

////////////////////////////////////////

parameter_value::parameter_value( const parameter_value &o )
	: _ptype( o._ptype )
{
	switch ( _ptype )
	{
		case PARAM_BOOL: _bval = o._bval; break;
		case PARAM_INT: _ival = o._ival; break;
		case PARAM_FLOAT: _dval = o._dval; break;
		case PARAM_STRING: new (&_sval) std::string( o._sval ); break;
		case PARAM_UNKNOWN: break;
	}
}

////////////////////////////////////////

parameter_value &
parameter_value::operator=( const parameter_value &o )
{
	if ( this != &o )
	{
		destroy();
		_ptype = o._ptype;
		switch ( _ptype )
		{
			case PARAM_BOOL: _bval = o._bval; break;
			case PARAM_INT: _ival = o._ival; break;
			case PARAM_FLOAT: _dval = o._dval; break;
			case PARAM_STRING: new (&_sval) std::string( o._sval ); break;
			case PARAM_UNKNOWN: break;
		}
	}
	return *this;
}

////////////////////////////////////////

parameter_value::parameter_value( parameter_value &&o )
	: _ptype( o._ptype )
{
	switch ( _ptype )
	{
		case PARAM_BOOL: _bval = o._bval; break;
		case PARAM_INT: _ival = o._ival; break;
		case PARAM_FLOAT: _dval = o._dval; break;
		case PARAM_STRING: new (&_sval) std::string( std::move( o._sval ) ); break;
		case PARAM_UNKNOWN: break;
	}
}

////////////////////////////////////////

parameter_value &
parameter_value::operator=( parameter_value &&o )
{
	if ( this != &o )
	{
		destroy();
		_ptype = o._ptype;
		switch ( _ptype )
		{
			case PARAM_BOOL: _bval = o._bval; break;
			case PARAM_INT: _ival = o._ival; break;
			case PARAM_FLOAT: _dval = o._dval; break;
			case PARAM_STRING: new (&_sval) std::string( std::move( o._sval ) ); break;
			case PARAM_UNKNOWN: break;
		}
	}
	return *this;
}

////////////////////////////////////////

bool parameter_value::as_bool( void ) const
{
	if ( _ptype == PARAM_BOOL )
		return _bval;
	if ( _ptype == PARAM_INT )
		return _ival != 0;

	throw_logic( "Request for bool from non-bool / convertible parameter value" );
}

////////////////////////////////////////

int64_t parameter_value::as_int( void ) const
{
	if ( _ptype == PARAM_BOOL )
		return _ptype ? 1 : 0;

	if ( _ptype != PARAM_INT )
		throw_logic( "Request for int from non-int / convertible parameter value" );
	return _ival;
}

////////////////////////////////////////

double parameter_value::as_float( void ) const
{
	if ( _ptype != PARAM_FLOAT )
		throw_logic( "Request for float from non-float parameter value" );
	return _ival;
}

////////////////////////////////////////

const std::string &parameter_value::as_string( void ) const
{
	if ( _ptype != PARAM_STRING )
		throw_logic( "Request for string from non-string parameter value" );
	return _sval;
}

////////////////////////////////////////

void parameter_value::set_from_string( const std::string &v )
{
	switch ( _ptype )
	{
		case PARAM_BOOL:
		{
			std::string lc = base::to_lower( v );
			if ( lc == "t" || lc == "true" || lc == "y" || lc == "yes" )
				_bval = true;
			else if ( lc == "f" || lc == "false" || lc == "n" || lc == "no" )
				_bval = false;
			else
				_bval = stoi( lc ) > 0;
			break;
		}
		case PARAM_INT: _ival = stoll( v ); break;
		case PARAM_FLOAT: _dval = stod( v ); break;
		case PARAM_STRING: _sval = v; break;
		case PARAM_UNKNOWN:
			throw_logic( "Attempt to set unknown parameter value type" );
	}
}

////////////////////////////////////////

void
parameter_value::destroy( void )
{
	if ( _ptype == PARAM_STRING )
		_sval.~basic_string();
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &os, const parameter_value &pv )
{
	switch ( pv.type() )
	{
		case PARAM_BOOL: os << std::boolalpha << pv.as_bool() << std::noboolalpha; break;
		case PARAM_INT: os << pv.as_int(); break;
			// todo: precision?
		case PARAM_FLOAT: os << pv.as_float(); break;
		case PARAM_STRING: os << pv.as_string(); break;
		case PARAM_UNKNOWN: os << "<invalid>"; break;
	}
	return os;
}

////////////////////////////////////////

//parameter_definition::parameter_definition( base::cstring name, parameter_type pt )
//	: _name( name ), _type( pt )
//{
//}

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, bool defVal )
	: _name( name ),
	  _type( PARAM_BOOL ),
	  _def_val( defVal )
{
}

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, int64_t minVal, int64_t maxVal, int64_t defVal )
	: _name( name ),
	  _type( PARAM_INT ),
	  _allowed_vals( std::make_pair( minVal, maxVal ) ),
	  _def_val( defVal )
{
}

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, double minVal, double maxVal, double defVal )
	: _name( name ),
	  _type( PARAM_FLOAT ),
	  _allowed_vals( std::make_pair( minVal, maxVal ) ),
	  _def_val( defVal )
{
}

////////////////////////////////////////

parameter_definition::parameter_definition( base::cstring name, std::vector<std::string> vals, base::cstring defVal )
		: _name( name ),
		  _type( PARAM_STRING ),
		  _allowed_vals( std::move( vals ) ),
		  _def_val( std::move( defVal ) )
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

parameter_value
parameter_definition::default_value( void ) const
{
	return _def_val;
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &os, const parameter_definition &pd )
{
	os << pd.name() << ": <" << as_string( pd.type() ) << " default "
	   << pd.default_value() << ") " << pd.help();
	return os;
}

////////////////////////////////////////

parameter_set initialize_parameters(
	const parameter_definitions &params,
	const std::string &option )
{
	parameter_set r;
	for ( auto &p: params )
		r[p.name()] = p.default_value();
	if ( ! option.empty() )
	{
		auto opts = base::split( option, ",", true );
		for ( auto &o: opts )
		{
			auto nv = base::split( o, "=", false );
			if ( nv.size() == 2 )
			{
				auto ri = r.find( nv[0] );
				if ( ri == r.end() )
				{
					std::stringstream errstr;
					for ( auto &p: params )
						errstr << p << '\n';

					throw_runtime( "Invalid option '{0}' requested, valid options are:\n{1}",
								   nv[0], errstr.str() );
				}
			}
			else
				throw_runtime( "Invalid media parameter option '{0}' in '{1}', unable to parse name=value", o, option );
		}
	}
	return r;
}

////////////////////////////////////////

} // media



