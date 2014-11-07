
#include "unifier.h"
#include <algorithm>

namespace imgproc
{

////////////////////////////////////////

void unifier::operator()( const type_variable &x, const type_variable &y )
{
	if ( x != y )
		eliminate( x, y );
}

////////////////////////////////////////

void unifier::operator()( const type_variable &x, const type_operator &y )
{
	if ( occurs( y, x ) )
		throw_runtime( "..." );
	eliminate( x, y );
}

////////////////////////////////////////

void unifier::operator()( const type_operator &x, const type_variable &y )
{
	if ( occurs( x, y ) )
		throw_runtime( "..." );
	eliminate( y, x );
}

////////////////////////////////////////

void unifier::operator()( const type_operator &x, const type_operator &y )
{
	if ( !x.compare_type( y ) )
		throw_runtime( "type mismatch: {0} and {1}", x, y );

	for ( auto xi = x.begin(), yi = y.begin(); xi != x.end(); ++xi, ++yi )
		_stack.emplace_back( *xi, *yi );
}

////////////////////////////////////////

type unifier::get( const type &tv )
{
	type next( tv );
	while ( next.is<type_variable>() )
	{
		auto tmp = next.get<type_variable>();
		auto i = _substitution.find( tmp );
		if ( i == _substitution.end() )
			next.clear();
		else
			next = i->second;
	}

	return next;
}

////////////////////////////////////////

void unifier::unify( void )
{
	while ( !_stack.empty() )
	{
		type x = std::move( _stack.back().first );
		type y = std::move( _stack.back().second );
		_stack.pop_back();
		visit( *this, x, y );
	}
}

////////////////////////////////////////

void unifier::eliminate( const type_variable &x, const type &y )
{
	for ( auto &i: _stack )
	{
		replace( i.first, x, y );
		replace( i.second, x, y );
	}

	for ( auto &i: _substitution )
		replace( i.second, x, y );

	_substitution[x] = y;
}

////////////////////////////////////////

void unifier::replace( type &x, const type_variable &target, const type &replacement )
{
	if ( x.is<type_operator>() )
	{
		auto &op = x.get<type_operator>();
		for( auto &o: op )
			replace( o, target, replacement );
	}
	else
	{
		auto &var = x.get<type_variable>();
		if ( var == target )
			x = replacement;
	}
}

////////////////////////////////////////

bool unifier::occurs( const type &haystack, const type_variable &needle )
{
	bool result = false;

	if ( haystack.is<type_operator>() )
	{
		auto &op = haystack.get<type_operator>();
		result = std::any_of( op.begin(), op.end(), [&]( const type &x )
		{
			return occurs( x, needle );
		} );
	}
	else
	{
		auto &var = haystack.get<type_variable>();
		result = ( var == needle );
	}

	return result;
}

////////////////////////////////////////

}

