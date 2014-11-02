
#pragma once

#include "data_type.h"
#include "token.h"
#include <vector>

namespace imgproc
{

////////////////////////////////////////

class decl
{
public:
	decl( std::u32string n )
		: _name( std::move( n ) )
	{
	}

	decl( void )
	{
	}

	void set_name( std::u32string n )
	{
		_name = std::move( n );
	}

	void add_arg( var_type t )
	{
		_args.emplace_back( t );
	}

	const std::u32string &name( void ) const
	{
		return _name;
	}

	const std::vector<var_type> &args( void ) const
	{
		return _args;
	}

	void parse( iterator &token );

private:
	std::u32string _name;
	std::vector<var_type> _args;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const decl &d );

////////////////////////////////////////

}

