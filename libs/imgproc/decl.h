
#pragma once

#include "type.h"
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

	void add_arg( type_operator t )
	{
		_type.add( t );
	}

	const std::u32string &name( void ) const
	{
		return _name;
	}

	const type_operator &type( void ) const
	{
		return _type;
	}

	void parse( iterator &token );

	const type_operator &get_type( void ) const
	{
		return _type;
	}
	

private:
	type_operator parse_type( iterator &token );

	std::u32string _name;
	type_operator _type;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const decl &d );

////////////////////////////////////////

}

