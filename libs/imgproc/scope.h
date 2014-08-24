
#pragma once

#include <map>
#include <memory>
#include <utf/utf.h>
#include "variable.h"

namespace imgproc
{

class func;

////////////////////////////////////////

class function_table
{
public:
	function_table( const std::function<type(const std::u32string &, const std::vector<type> &)> &c )
		: _compile( c )
	{
	}

	bool has( const std::u32string &n )
	{
		return bool( get( n ) );
	}

	std::shared_ptr<func> get( const std::u32string &n )
	{
		auto f = _functions.find( n );
		if ( f == _functions.end() )
			return std::shared_ptr<func>();
		return f->second;
	}

	void add( const std::u32string &n, const std::shared_ptr<func> &f )
	{
		if ( get( n ) )
			throw_runtime( "function \"{0}\" already exists", n );
		_functions[n] = f;
	}

	type compile( const std::u32string &name, const std::vector<type> &args )
	{
		return _compile( name, args );
	}

private:

	std::function<type(const std::u32string &, const std::vector<type> &)> _compile;
	std::map<std::u32string,std::shared_ptr<func>> _functions;
};


////////////////////////////////////////

class scope
{
public:
	scope( void )
	{
	}

	scope( const std::shared_ptr<scope> &p )
		: _parent( p ), _functions( p->functions() )
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
				throw_runtime( "variable \"{0}\" not found", n );
		}
		return v->second;
	}

	void add( const std::u32string &n, const type &t )
	{
		_variables.emplace( std::make_pair( n, variable( n, t ) ) );
	}

	const std::shared_ptr<function_table> &functions( void ) const
	{
		return _functions;
	}

	std::shared_ptr<function_table> &functions( void )
	{
		return _functions;
	}

	void set_functions( const std::shared_ptr<function_table> &f )
	{
		_functions = f;
	}

private:
	std::shared_ptr<scope> _parent;
	std::map<std::u32string,variable> _variables;
	std::shared_ptr<function_table> _functions;
};

////////////////////////////////////////

}

