
#pragma once

#include "expr.h"

namespace imgproc
{

////////////////////////////////////////

class function
{
public:
	function( std::u32string n )
		: _name( std::move( n ) )
	{
	}

	template<typename ...Args>
	function( std::u32string n, const Args &...args )
		: _name( std::move( n ) )
	{
		std::array<std::u32string,sizeof...(args)> list
		{{
			args...
		}};

		for ( auto arg: list )
			add_arg( arg, std::u32string() );
	}

	function( void )
	{
	}

	function( const function &f, const std::shared_ptr<expr> &result )
		: _name( f.name() ), _args( f.args() ), _mods( f.arg_modifiers() ), _result( result )
	{
	}

	void add_arg( std::u32string a, std::u32string mod )
	{
		_args.push_back( std::move( a ) );
		_mods.push_back( std::move( mod ) );
	}

	void set_result( const std::shared_ptr<expr> &r )
	{
		precondition( r, "missing result" );
		_result = r;
	}

	const std::u32string &name( void ) const
	{
		return _name;
	}

	const std::vector<std::u32string> &args( void ) const
	{
		return _args;
	}

	const std::vector<std::u32string> &arg_modifiers( void ) const
	{
		return _mods;
	}

	const std::shared_ptr<expr> result( void ) const { return _result; }

	std::shared_ptr<function> clone( void ) const;

private:
	std::u32string _name;
	std::vector<std::u32string> _args;
	std::vector<std::u32string> _mods;
	std::shared_ptr<expr> _result;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const function &f );

////////////////////////////////////////

}

