
#pragma once

#include <map>
#include <memory>
#include <utf/utf.h>
#include "variable.h"

namespace imgproc
{

////////////////////////////////////////

class scope
{
public:
	scope( void )
	{
	}

	scope( const std::shared_ptr<scope> &p )
		: _parent( p )
	{
	}

	const variable &get( const std::u32string &n ) 
	{
		auto v = _variables.find( n );
		if ( v == _variables.end() )
		{
			if ( _parent )
				return _parent->get( n );
			else
				throw_runtime( "variable {0} not found", n );
		}
		return v->second;
	}

	void add( const std::u32string &n, const type &t )
	{
		_variables.emplace( std::make_pair( n, variable( n, t ) ) );
	}

private:
	std::shared_ptr<scope> _parent;
	std::map<std::u32string,variable> _variables;
};

////////////////////////////////////////

}

