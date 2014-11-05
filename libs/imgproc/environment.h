
#pragma once

#include "unifier.h"
#include "expr.h"
#include <utf/utf.h>

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

	type operator()( const integer_expr &e )
	{
		// TODO pick the "minimum" type possible
		return type_operator( pod_type::INT64, 0 );
	}

	type operator()( const floating_expr &e )
	{
		// TODO pick the "minimum" type possible
		return type_operator( pod_type::FLOAT32, 0 );
	}

	type operator()( const identifier_expr &e )
	{
		auto i = _env.find( e.value() );
		if ( i == _env.end() )
			throw_runtime( "undefined symbol {0}", e.value() );

		// TODO
		return i->second;
	}

	type operator()( const prefix_expr &e )
	{
		// Prefix operation keep the same type.
		return visit( e.expression() );
	}

	type operator()( const postfix_expr &e )
	{
		// Postfix operation keep the same type.
		return visit( e.expression() );
	}

	type operator()( const infix_expr &e )
	{
		auto t1 = visit( e.expression1() );
		auto t2 = visit( e.expression2() );
		_unify.add_constraint( t1, t2 );
		return t1;
	}

	type operator()( const circumfix_expr &e )
	{
		// Circumfix keep the same type
		return visit( e.expression() );
	}

	type operator()( const postcircumfix_expr &e )
	{
		// TODO should be checking expression2
		return visit( e.expression1() );
	}

	type operator()( const call_expr &e )
	{
		throw_not_yet();
	}

	type operator()( const if_expr &e )
	{
		auto c = base::visit<type>( *this, *e.condition() );
		_unify.add_constraint( c, type_operator( pod_type::BOOLEAN, 0 ) );

		auto e1 = visit( e.when_true() );
		auto e2 = visit( e.when_false() );
		_unify.add_constraint( e1, e2 );

		return e1;
	}

	type operator()( const range_expr &e )
	{
		return type_operator( pod_type::INT64, 0 );
	}

	type operator()( const for_expr &e )
	{
		// TODO
		throw_not_yet();
	}

	type operator()( const assign_expr &e )
	{
		// TODO
		throw_not_yet();
	}

	type operator()( const lambda_expr &e )
	{
		// TODO
		throw_not_yet();
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	type operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
		return type();
	}

	type visit( const std::shared_ptr<expr> &e )
	{
		auto t = base::visit<type>( *this, *e );
		e->set_type( std::move( t ) );
		return e->get_type();
	}

private:

	unifier _unify;
	std::map<std::u32string,type> _env;
};

////////////////////////////////////////

std::shared_ptr<expr> infer( const function &f, const std::vector<var_type> &arg_types );

////////////////////////////////////////


}

