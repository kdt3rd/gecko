
#pragma once

#include <vector>
#include <map>
#include <typeindex>
#include <base/variant.h>
#include "type.h"

namespace imgproc
{

////////////////////////////////////////

class unifier
{
public:
	typedef std::pair<type, type> constraint;

	void operator()( const type_variable &x, const type_variable &y );
	void operator()( const type_variable &x, const type_primary &y );
	void operator()( const type_variable &x, const type_callable &y );
	void operator()( const type_primary &x, const type_variable &y );
	void operator()( const type_primary &x, const type_primary &y );
	void operator()( const type_primary &x, const type_callable &y );
	void operator()( const type_callable &x, const type_variable &y );
	void operator()( const type_callable &x, const type_primary &y );
	void operator()( const type_callable &x, const type_callable &y );

	// Compile-time check for any missing operator() implementation
	template<typename T1, typename T2>
	void operator()( T1 a, T2 b )
	{
		static_assert( base::always_false<T1,T2>::value, "missing operator() for unifier types" );
	}

	void add_constraint( const type &t1, const type &t2 );

	template<typename Iterator>
	void add_constraint( const Iterator &first, const Iterator &last )
	{
		_stack.insert( _stack.end(), first, last );
	}

	template<typename Range>
	void add_constraint( const Range &range )
	{
		add_constraint( range.begin(), range.end() );
	}

	type get( const type &tv );

	void unify( void );

	void print_constraints( std::ostream &out )
	{
		for ( auto &i: _stack )
			out << i.first << " = " << i.second << '\n';
	}

	void print_substitutions( std::ostream &out )
	{
		for ( auto &i: _substitution )
			out << i.first << " = " << i.second << '\n';
	}

private:
	void eliminate( const type_variable &x, const type &y );
	void replace( type &x, const type_variable &target, const type &replacement );
	bool occurs( const type &haystack, const type_variable &needle );

	std::vector<constraint> _stack;
	std::map<type_variable, type> _substitution;
};

////////////////////////////////////////

}

