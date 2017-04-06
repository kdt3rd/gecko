//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "environment.h"
#include "function.h"
#include <base/reverse.h>
#include <base/scope_guard.h>

namespace imgproc
{

////////////////////////////////////////

class assign_type
{
public:
	assign_type( unifier &u, std::map<const void*,std::shared_ptr<expr>> &exprs )
		: _unify( u ), _exprs( exprs )
	{
	}

	void operator()( const integer_expr & /*e*/ )
	{
	}

	void operator()( const floating_expr & /*e*/ )
	{
	}

	void operator()( const identifier_expr & /*e*/ )
	{
	}

	void operator()( const prefix_expr &e )
	{
		visit( e.expression() );
	}

	void operator()( const postfix_expr &e )
	{
		visit( e.expression() );
	}

	void operator()( const infix_expr &e )
	{
		visit( e.expression1() );
		visit( e.expression2() );
	}

	void operator()( const circumfix_expr &e )
	{
		visit( e.expression() );
	}

	void operator()( const postcircumfix_expr &e )
	{
		visit( e.expression1() );
		visit( e.expression2() );
	}

	void operator()( const call_expr &e )
	{
		auto i = _exprs.find( &e );
		if ( i != _exprs.end() )
			visit( i->second );
	}

	void operator()( const if_expr &e )
	{
		visit( e.condition() );
		visit( e.when_true() );
		visit( e.when_false() );
	}

	void operator()( const range_expr & /*e*/ )
	{
	}

	void operator()( const for_expr &e )
	{
		visit( e.result() );
	}

	void operator()( const assign_expr &e )
	{
		visit( e.expression() );
		visit( e.next() );
	}

	void operator()( const lambda_expr & /*e*/ )
	{
		// TODO
		throw_not_yet();
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	void operator()( T /*a*/ )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
	}

	void visit( const std::shared_ptr<expr> &e )
	{
		if ( e->get_type().is<type_variable>() )
			e->set_type( _unify.get( e->get_type().get<type_variable>() ) );

		if ( !e->get_type().valid() )
			throw_runtime( "could not infer expression: {0}", e );

		base::visit<void>( *this, *e );
	}

private:
	unifier &_unify;
	std::map<const void*,std::shared_ptr<expr>> _exprs;
};

////////////////////////////////////////

environment::environment( std::map<std::u32string,std::shared_ptr<function>> &f )
	: _funcs( f )
{
}

////////////////////////////////////////

type environment::operator()( const integer_expr &e )
{
	uint64_t v = e.integer();

	if ( v <= std::numeric_limits<uint8_t>::max() )
		return type_primary( pod_type::UINT8 );

	if ( v <= std::numeric_limits<uint16_t>::max() )
		return type_primary( pod_type::UINT16 );

	if ( v <= std::numeric_limits<uint32_t>::max() )
		return type_primary( pod_type::UINT32 );

	return type_primary( pod_type::UINT64 );
}

////////////////////////////////////////

type environment::operator()( const floating_expr & /*e*/ )
{
	// TODO when should we use FLOAT32?
	return type_primary( pod_type::FLOAT64 );
}

////////////////////////////////////////

type environment::operator()( const identifier_expr &e )
{
	const type &t( find_scope( e.value() ) );
	if ( t.valid() )
		return t;

	auto f = _funcs.find( e.value() );
	if ( f != _funcs.end() )
	{
		type_callable result( new_type(), type_callable::FUNCTION );
		for ( size_t i = 0; i < f->second->args().size(); ++i )
			result.add_arg( new_type() );
		auto ex = unify( *f->second, result );
		_func_exprs[&e] = ex;
		_unify.add_constraint( result.get_result(), ex->get_type() );
		return result;
	}

	throw_runtime( "undefined symbol {0}", e.value() );
}

////////////////////////////////////////

type environment::operator()( const prefix_expr &e )
{
	type result = new_type();
	_unify.add_constraint( result, visit( e.expression() ) );
	return result;
}

////////////////////////////////////////

type environment::operator()( const postfix_expr &e )
{
	type result = new_type();
	auto t = visit( e.expression() );
	_unify.add_constraint( result, t );
	return result;
}

////////////////////////////////////////

type environment::operator()( const infix_expr &e )
{
	type result = new_type();
	_unify.add_constraint( result, visit( e.expression1() ) );
	_unify.add_constraint( result, visit( e.expression2() ) );
	return result;
}

////////////////////////////////////////

type environment::operator()( const circumfix_expr &e )
{
	return visit( e.expression() );
//	type result = new_type();
//	auto t = visit( e.expression() );
//	_unify.add_constraint( result, t );
//	return result;
}

////////////////////////////////////////

type environment::operator()( const postcircumfix_expr &e )
{
	type result = new_type();
	auto t1 = visit( e.expression1() );
	auto t2 = visit( e.expression2() );
	_unify.add_constraint( result, t2 );
	_unify.add_constraint( result, t1 );
	return result;
}

////////////////////////////////////////

type environment::operator()( const call_expr &e )
{
	type result = new_type();

	// Get argument types
	std::vector<type> args;
	for ( auto &arg: e.arguments() )
		args.push_back( visit( arg ) );

	auto ty = find_scope( e.function() );
	if ( !ty.valid() )
	{
		auto f = _funcs.find( e.function() );
		if ( f != _funcs.end() )
		{
			type_callable ftype( result, type_callable::FUNCTION );
			for ( size_t i = 0; i < f->second->args().size(); ++i )
				ftype.add_arg( new_type() );
			ty = ftype;
			_func_exprs[&e] = unify( *f->second, ftype );
			_unify.add_constraint( result, _func_exprs[&e]->get_type() );
		}
	}

	if ( ty.valid() )
	{
		if ( ty.is<type_callable>() )
		{
			type_callable &top = ty.get<type_callable>();
			if ( top.size() != args.size() )
				throw_runtime( "different number of arguments ({0} and {1})", top.size(), args.size() );

			for ( size_t i = 0; i < top.size(); ++i )
				_unify.add_constraint( top.at( i ), args[i] );
			_unify.add_constraint( result, top.get_result() );
		}
		else if ( ty.is<type_variable>() )
		{
			type_callable call( result );
			for ( auto &t: args )
				call.add_arg( t );
			_unify.add_constraint( ty, call );
		}
		else
			_unify.add_constraint( result, ty );
	}
	else
		throw_runtime( "function {0} not found", e.function() );

	return result;
}

////////////////////////////////////////

type environment::operator()( const if_expr &e )
{
	auto c = visit( e.condition() );
	_unify.add_constraint( c, type_primary( pod_type::BOOLEAN ) );

	type result = new_type();
	_unify.add_constraint( result, visit( e.when_true() ) );
	_unify.add_constraint( result, visit( e.when_false() ) );

	return result;
}

////////////////////////////////////////

type environment::operator()( const range_expr & /*e*/ )
{
	return type_primary( pod_type::INT64 );
}

////////////////////////////////////////

type environment::operator()( const for_expr &e )
{
	_env.emplace_back();
	for ( auto &v: e.ranges() )
		add_scope( v.variable(), type_primary( pod_type::INT64 ) );

	type result = visit( e.result() );
	_env.pop_back();

	return result;
}

////////////////////////////////////////

type environment::operator()( const assign_expr &e )
{
	auto t = visit( e.expression() );
	add_scope( e.variable(), t );
	return visit( e.next() );
}

////////////////////////////////////////

type environment::operator()( const lambda_expr & /*e*/ )
{
	/*
	type_callable result( visit( e.result() ), type_callable::FUNCTION );

	for ( auto &arg: e.args() )
		result.add_arg(  );

	return result;
	*/
	throw_not_yet();
}

////////////////////////////////////////

type environment::find_scope( const std::u32string &name )
{
	for ( auto &i: base::reverse( _env ) )
	{
		auto j = i.find( name );
		if ( j != i.end() )
			return j->second;
	}

	return type();
}

////////////////////////////////////////

void environment::add_scope( const std::u32string &name, const type &ty )
{
	for ( auto &i: base::reverse( _env ) )
	{
		auto j = i.find( name );
		if ( j != i.end() )
			throw_runtime( "variable {0} already defined", name );
	}

	_env.back()[name] = ty;
}

////////////////////////////////////////

type environment::visit( const std::shared_ptr<expr> &e )
{
	_current.push_back( e );
	on_scope_exit { _current.pop_back(); };

	auto t = base::visit<type>( *this, *e );
	e->set_type( std::move( t ) );
	return e->get_type();
}

////////////////////////////////////////

type environment::new_type( void )
{
	precondition( !_current.empty(), "no expression to create type from" );
	type_variable result( ++_type_id );
	_var_exprs[result.id()] = _current.back();
	return result;
}

////////////////////////////////////////

type environment::join( const type_primary &t1, const type_primary &t2 )
{
	if ( t1.get_type() > t2.get_type() )
		return join( t2, t1 );

	if ( t1.get_type() == t2.get_type() )
		return t1;

	switch ( t2.get_type() )
	{
		case pod_type::UINT8:
			if ( is_unsigned( t1.get_type() ) )
				return t2;
			else if ( is_signed( t1.get_type() ) )
				return type_primary( pod_type::INT8 );
			break;

		case pod_type::UINT16:
			if ( is_unsigned( t1.get_type() ) )
				return t2;
			else if ( is_signed( t1.get_type() ) )
				return type_primary( pod_type::INT16 );
			break;

		case pod_type::UINT32:
			if ( is_unsigned( t1.get_type() ) )
				return t2;
			else if ( is_signed( t1.get_type() ) )
				return type_primary( pod_type::INT32 );
			break;

		case pod_type::UINT64:
			if ( is_unsigned( t1.get_type() ) )
				return t2;
			else if ( is_signed( t1.get_type() ) )
				return type_primary( pod_type::INT64 );
			break;

		case pod_type::INT8:
		case pod_type::INT16:
		case pod_type::INT32:
		case pod_type::INT64:
			if ( is_signed( t1.get_type() ) || is_unsigned( t1.get_type() ) )
				return t2;
			break;

		case pod_type::FLOAT16:
		case pod_type::FLOAT32:
		case pod_type::FLOAT64:
			if ( is_signed( t1.get_type() ) || is_unsigned( t1.get_type() ) )
				return t2;
			break;

		case pod_type::BOOLEAN:
			return t2;

		case pod_type::UNKNOWN:
			break;
	}

	throw_runtime( "cannot join {0} and {1}", t1, t2 );
}

////////////////////////////////////////

std::shared_ptr<expr> environment::infer( const function &f, std::vector<type> &arg_types )
{
	precondition( arg_types.size() == f.args().size(), "mismatch for argument types {0} vs {1} for function {2}", arg_types.size(), f.args().size(), f.name() );

	std::shared_ptr<expr> result = unify( f, arg_types );
	_unify.unify();

	assign_type a( _unify, _func_exprs );
	a.visit( result );
	return result;
}

////////////////////////////////////////

std::shared_ptr<expr> environment::unify( const function &f, std::vector<type> &arg_types )
{
	precondition( arg_types.size() == f.args().size(), "mismatch for argument types {0} vs {1} for function {2}", arg_types.size(), f.args().size(), f.name() );

	std::shared_ptr<expr> result = f.result()->clone();

	// create a new scope with environment variables.
	scope newscope;
	for ( size_t i = 0; i < arg_types.size(); ++i )
	{
		const std::u32string &aname = f.args()[i];
		newscope[aname] = arg_types[i];
	}

	std::vector<scope> old;
	old.push_back( std::move( newscope ) );

	std::swap( _env, old );
	on_scope_exit { std::swap( _env, old ); };

	visit( result );
	return result;
}

////////////////////////////////////////

std::shared_ptr<expr> environment::unify( const function &f, type_callable &call )
{
	precondition( call.size() == f.args().size(), "mismatch for argument types {0} vs {1} for function {2}", call.size(), f.args().size(), f.name() );
	return unify( f, call.args() );
}

////////////////////////////////////////

}

