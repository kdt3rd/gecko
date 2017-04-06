//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <vector>
#include <map>
#include <typeindex>
#include <memory>
#include <base/variant.h>
#include "data_type.h"

namespace imgproc
{

class type;

////////////////////////////////////////

class type_variable
{
public:
	type_variable( void )
		: _id()
	{
	}

	type_variable( size_t i )
		: _id( i )
	{
	}

	size_t id( void ) const
	{
		return _id;
	}

	bool operator==( const type_variable &v ) const
	{
		return id() == v.id();
	}

	bool operator!=( const type_variable &v ) const
	{
		return id() != v.id();
	}

	bool operator<( const type_variable &v ) const
	{
		return id() < v.id();
	}

private:
	size_t _id;
};

////////////////////////////////////////

class type_primary
{
public:
	type_primary( void )
	{
	}

	// @brief Unknown type
	type_primary( pod_type t )
		: _type( t )
	{
	}

	bool is_compatible( const type_primary &o ) const
	{
		if ( _type == pod_type::UNKNOWN || o._type == pod_type::UNKNOWN )
			return true;

		if ( _type == o._type )
			return true;

		return false;
	}

	pod_type get_type( void ) const
	{
		return _type;
	}

	void set_type( pod_type t )
	{
		_type = t;
	}

private:
	pod_type _type = pod_type::UNKNOWN;
};

////////////////////////////////////////

class type_callable
{
public:
	enum call_type
	{
		IMAGE,
		FUNCTION,
		UNKNOWN
	};

	type_callable( void )
	{
	}

	type_callable( type result, call_type c = UNKNOWN );

	std::vector<type>::iterator begin( void )
	{
		return _args.begin();
	}

	std::vector<type>::iterator end( void )
	{
		return _args.end();
	}

	std::vector<type>::const_iterator begin( void ) const
	{
		return _args.begin();
	}

	std::vector<type>::const_iterator end( void ) const
	{
		return _args.end();
	}

	bool empty( void ) const
	{
		return _args.empty();
	}

	size_t size( void ) const
	{
		return _args.size();
	}

	const type &at( size_t i ) const
	{
		return _args.at( i );
	}

	void add_arg( type t );

	type &get_result( void )
	{
		return *_result;
	}

	const type &get_result( void ) const
	{
		return *_result;
	}

	call_type get_call_type( void ) const
	{
		return _call;
	}

	std::vector<type> &args( void )
	{
		return _args;
	}

private:
	call_type _call = UNKNOWN;
	std::shared_ptr<type> _result;
	std::vector<type> _args;
};

////////////////////////////////////////

class type : public base::variant<type_variable,type_primary,type_callable>
{
public:
	using variant::variant;

};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const type &t );

////////////////////////////////////////

}

