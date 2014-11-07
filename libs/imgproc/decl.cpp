
#include <iterator>
#include "decl.h"

namespace imgproc
{

////////////////////////////////////////

void
decl::parse( iterator &token )
{
	token.next();

	if ( token.type() != TOK_IDENTIFIER )
		throw_runtime( "expected function name to start declaration" );
	_name = std::move( token.value() );
	token.next();

	if ( token.type() != TOK_PAREN_START )
		throw_runtime( "expected '(' to start function arguments" );
	token.next();

	while ( token.type() != TOK_PAREN_END )
	{
		_type.add( parse_type( token ) );

		if ( token.type() == TOK_COMMA )
			token.next();
		else if ( token.type() != TOK_PAREN_END )
			throw_runtime( "expected ',' or ')' for arguments, got '{0}'", token );
	}

	if ( token.type() != TOK_PAREN_END )
		throw_runtime( "expected ')' to end function arguments" );
	token.next();
}

////////////////////////////////////////

type_operator
decl::parse_type( iterator &token )
{
	type_operator result;

	if ( token.type() != TOK_IDENTIFIER && token.type() != TOK_FUNCTION )
		throw_runtime( "expected type name, got '{0}'", token );
	pod_type pt = type_enum( token.value() );
	if ( pt == pod_type::UNKNOWN )
		throw_runtime( "expected type name, got '{0}'", token );
	token.next();

	result.set_type( pt );

	size_t dims = 0;
	if ( token.type() == TOK_PAREN_START )
	{
		token.next();
		while ( token.type() == TOK_IDENTIFIER )
		{
			type_operator arg = parse_type( token );
			if ( pt == pod_type::FUNCTION )
				result.add( arg );
			else
			{
				if ( arg.base_type() != pod_type::UINT64 || arg.dimensions() != 0 )
					throw_runtime( "expected uint64 for array index, got '{0}'", arg );
				++dims;
			}
			if ( token.type() == TOK_COMMA )
				token.next();
			else if ( token.type() != TOK_PAREN_END )
				throw_runtime( "expected ',' or ')' for arguments, got '{0}'", token );
		}

		if ( token.type() != TOK_PAREN_END )
			throw_runtime( "expected ')' to end arguments, got '{0}'", token );
		token.next();
	}
	else if ( pt == pod_type::FUNCTION )
		throw_runtime( "expected '(' to begin function arguments, got '{0}'", token );

	result.set_dimensions( dims );

	return result;
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const decl &d )
{
	bool first = true;
	out << d.name() << '(';
	for ( const auto &a: d.type() )
	{
		if ( !first )
			out << ',';
		first = false;
		out << a;
		/*
		switch ( a )
		{
			case 0: break;
			case 1: out << "(x)"; break;
			case 2: out << "(x,y)"; break;
			case 3: out << "(x,y,z)"; break;
			case 4: out << "(x,y,z,w)"; break;
			default: out << "(...)"; break;
		}
		*/
	}
	out << ')';
	return out;
}

////////////////////////////////////////

}

