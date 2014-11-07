
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

	type operator()( const integer_expr &e )
	{
		uint64_t v = e.integer();

		if ( v <= std::numeric_limits<uint8_t>::max() )
			return type_operator( pod_type::UINT8, 0 );

		if ( v <= std::numeric_limits<uint16_t>::max() )
			return type_operator( pod_type::UINT16, 0 );

		if ( v <= std::numeric_limits<uint32_t>::max() )
			return type_operator( pod_type::UINT32, 0 );

		return type_operator( pod_type::UINT64, 0 );
	}

	type operator()( const floating_expr &e )
	{
		// TODO when should we use FLOAT32?
		return type_operator( pod_type::FLOAT64, 0 );
	}

	type operator()( const identifier_expr &e )
	{
		auto i = _env.find( e.value() );
		if ( i == _env.end() )
			throw_runtime( "undefined symbol {0}", e.value() );
		return i->second;
	}

	type operator()( const prefix_expr &e )
	{
		type result = new_type();
		auto t = visit( e.expression() );
		_unify.add_constraint( result, t );
		return result;
	}

	type operator()( const postfix_expr &e )
	{
		type result = new_type();
		auto t = visit( e.expression() );
		_unify.add_constraint( result, t );
		return result;
	}

	type operator()( const infix_expr &e )
	{
		type result = new_type();
		auto t1 = visit( e.expression1() );
		auto t2 = visit( e.expression2() );
		_unify.add_constraint( result, t2 );
		_unify.add_constraint( result, t1 );
		return result;
	}

	type operator()( const circumfix_expr &e )
	{
		return visit( e.expression() );
		type result = new_type();
		auto t = visit( e.expression() );
		_unify.add_constraint( result, t );
		return result;
	}

	type operator()( const postcircumfix_expr &e )
	{
		type result = new_type();
		auto t1 = visit( e.expression1() );
		auto t2 = visit( e.expression2() );
		_unify.add_constraint( result, t2 );
		_unify.add_constraint( result, t1 );
		return result;
	}

	type operator()( const call_expr &e )
	{
		throw_not_yet();
	}

	type operator()( const if_expr &e )
	{
		type result = new_type();
		auto c = visit( e.condition() );
		_unify.add_constraint( c, type_operator( pod_type::BOOLEAN, 0 ) );

		auto e1 = visit( e.when_true() );
		auto e2 = visit( e.when_false() );
		_unify.add_constraint( result, e1 );
		_unify.add_constraint( result, e2 );

		return result;
	}

	type operator()( const range_expr &e )
	{
		return type_operator( pod_type::INT64, 0 );
	}

	type operator()( const for_expr &e )
	{
		auto t = visit( e.result() );

		// TODO
		throw_not_yet();
	}

	type operator()( const assign_expr &e )
	{
		type result = new_type();
		auto t = visit( e.expression() );
		_unify.add_constraint( result, t );

		this->operator[]( e.variable() ) = result;

		return result;
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

	void unify( std::shared_ptr<expr> &e );

	type visit( const std::shared_ptr<expr> &e );

private:
	type new_type( void )
	{
		return type_variable( ++_type_id );
	}

	size_t _type_id = 0;
	unifier _unify;
	std::map<std::u32string,type> _env;
};

////////////////////////////////////////

std::shared_ptr<expr> infer( const function &f, const std::vector<type_operator> &arg_types );

////////////////////////////////////////

}

