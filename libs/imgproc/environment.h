
#pragma once

#include "unifier.h"
#include "expr.h"
#include <utf/utf.h>
#include <limits>

namespace imgproc
{

class function;

////////////////////////////////////////

class environment
{
public:
	const type &operator[]( const std::u32string &n ) const
	{
		return _env.at( n );
	}

	type &operator[]( const std::u32string &n )
	{
		return _env[n];
	}

	type operator()( const integer_expr &e );
	type operator()( const floating_expr &e );
	type operator()( const identifier_expr &e );
	type operator()( const prefix_expr &e );
	type operator()( const postfix_expr &e );
	type operator()( const infix_expr &e );
	type operator()( const circumfix_expr &e );
	type operator()( const postcircumfix_expr &e );
	type operator()( const call_expr &e );
	type operator()( const if_expr &e );
	type operator()( const range_expr &e );
	type operator()( const for_expr &e );
	type operator()( const assign_expr &e );
	type operator()( const lambda_expr &e );

	// Compile-time check for any missing operator() implementation
	template<typename T>
	type operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
		return type();
	}

	void unify( std::shared_ptr<expr> &e );

	type visit( const std::shared_ptr<expr> &e );

private:
	type new_type( void )
	{
		return type_variable( ++_type_id );
	}

	type join( const type_operator &t1, const type_operator &t2 );

	size_t _type_id = 0;
	unifier _unify;
	std::map<std::u32string,type> _env;
};

////////////////////////////////////////

std::shared_ptr<expr> infer( const function &f, const std::vector<type_operator> &arg_types );

////////////////////////////////////////

}

