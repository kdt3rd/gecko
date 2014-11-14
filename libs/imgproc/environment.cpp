
#include "environment.h"
#include "function.h"

namespace imgproc
{

////////////////////////////////////////

class assign_type
{
public:
	assign_type( unifier &u )
		: _unify( u )
	{
	}

	void operator()( const integer_expr &e )
	{
	}

	void operator()( const floating_expr &e )
	{
	}

	void operator()( const identifier_expr &e )
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
	}

	void operator()( const if_expr &e )
	{
		visit( e.condition() );
		visit( e.when_true() );
		visit( e.when_false() );
	}

	void operator()( const range_expr &e )
	{
	}

	void operator()( const for_expr &e )
	{
		visit( e.result() );
	}

	void operator()( const assign_expr &e )
	{
		visit( e.expression() );
	}

	void operator()( const lambda_expr &e )
	{
		// TODO
		throw_not_yet();
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	void operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
	}

	void visit( const std::shared_ptr<expr> &e )
	{
		if ( e->get_type().is<type_variable>() )
			e->set_type( _unify.get( e->get_type().get<type_variable>() ) );
		base::visit<void>( *this, *e );
	}

private:
	unifier &_unify;
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
		return type_operator( pod_type::UINT8, 0 );

	if ( v <= std::numeric_limits<uint16_t>::max() )
		return type_operator( pod_type::UINT16, 0 );

	if ( v <= std::numeric_limits<uint32_t>::max() )
		return type_operator( pod_type::UINT32, 0 );

	return type_operator( pod_type::UINT64, 0 );
}

////////////////////////////////////////

type environment::operator()( const floating_expr &e )
{
	// TODO when should we use FLOAT32?
	return type_operator( pod_type::FLOAT64, 0 );
}

////////////////////////////////////////

type environment::operator()( const identifier_expr &e )
{
	auto i = _env.find( e.value() );
	if ( i == _env.end() )
	{
		auto f = _funcs.find( e.value() );
		if ( f != _funcs.end() )
		{
			return type_operator( pod_type::FUNCTION, 0 );
		}

		throw_runtime( "undefined symbol {0}", e.value() );
	}
	return i->second;
}

////////////////////////////////////////

type environment::operator()( const prefix_expr &e )
{
	type result = new_type();
	auto t = visit( e.expression() );
	_unify.add_constraint( result, t );
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
	auto t1 = visit( e.expression1() );
	auto t2 = visit( e.expression2() );

	_unify.unify();
	t1 = _unify.get( t1 );
	t2 = _unify.get( t2 );

	_unify.add_constraint( result, join( t1.get<type_operator>(), t2.get<type_operator>() ) );
	return result;
}

////////////////////////////////////////

type environment::operator()( const circumfix_expr &e )
{
	return visit( e.expression() );
	type result = new_type();
	auto t = visit( e.expression() );
	_unify.add_constraint( result, t );
	return result;
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
	std::vector<type_operator> args;
	for ( auto &arg: e.arguments() )
	{
		auto t1 = visit( arg );
		_unify.unify();
		t1 = _unify.get( t1 );
		args.push_back( t1.get<type_operator>() );
	}

	auto f = _funcs[e.function()];
	if ( f )
	{
		environment env( _funcs );
		auto e = env.infer( *f, args );
		return e->get_type();
	}

	throw_runtime( "function {0} not found", e.function() );
}

////////////////////////////////////////

type environment::operator()( const if_expr &e )
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

////////////////////////////////////////

type environment::operator()( const range_expr &e )
{
	return type_operator( pod_type::INT64, 0 );
}

////////////////////////////////////////

type environment::operator()( const for_expr &e )
{
	for ( auto &v: e.variables() )
	{
		if ( _env.find( v ) != _env.end() )
			throw_runtime( "variable {0} already defined", v );
		_env[v] = type_operator( pod_type::INT64, 0 );
	}

	auto t = visit( e.result() );
	return t;
}

////////////////////////////////////////

type environment::operator()( const assign_expr &e )
{
	auto t = visit( e.expression() );
	_unify.unify();
	t = _unify.get( t );

	if ( _env.find( e.variable() ) != _env.end() )
		throw_runtime( "variable {0} already defined", e.variable() );

	_env[e.variable()] = t;

	return visit( e.next() );
}

////////////////////////////////////////

type environment::operator()( const lambda_expr &e )
{
	// TODO
	throw_not_yet();
}

////////////////////////////////////////

void environment::unify( std::shared_ptr<expr> &e )
{
	_unify.unify();
	assign_type a( _unify );
	a.visit( e );
}

////////////////////////////////////////

type environment::visit( const std::shared_ptr<expr> &e )
{
	auto t = base::visit<type>( *this, *e );
	e->set_type( std::move( t ) );
	return e->get_type();
}

////////////////////////////////////////

type environment::join( const type_operator &t1, const type_operator &t2 )
{
	if ( t1.base_type() > t2.base_type() )
		return join( t2, t1 );

	precondition( t1.dimensions() == t2.dimensions(), "cannot join {0} and {1}", t1, t2 );

	if ( t1.base_type() == t2.base_type() )
		return t1;

	switch ( t2.base_type() )
	{
		case pod_type::UINT8:
			if ( is_unsigned( t1.base_type() ) )
				return t2;
			else if ( is_signed( t1.base_type() ) )
				return type_operator( pod_type::INT8, t1.dimensions() );
			break;

		case pod_type::UINT16:
			if ( is_unsigned( t1.base_type() ) )
				return t2;
			else if ( is_signed( t1.base_type() ) )
				return type_operator( pod_type::INT16, t1.dimensions() );
			break;

		case pod_type::UINT32:
			if ( is_unsigned( t1.base_type() ) )
				return t2;
			else if ( is_signed( t1.base_type() ) )
				return type_operator( pod_type::INT32, t1.dimensions() );
			break;

		case pod_type::UINT64:
			if ( is_unsigned( t1.base_type() ) )
				return t2;
			else if ( is_signed( t1.base_type() ) )
				return type_operator( pod_type::INT64, t1.dimensions() );
			break;

		case pod_type::INT8:
		case pod_type::INT16:
		case pod_type::INT32:
		case pod_type::INT64:
			if ( is_signed( t1.base_type() ) || is_unsigned( t1.base_type() ) )
				return t2;
			break;

		case pod_type::FLOAT16:
		case pod_type::FLOAT32:
		case pod_type::FLOAT64:
			if ( is_signed( t1.base_type() ) || is_unsigned( t1.base_type() ) )
				return t2;
			break;

		default:
			break;
	}

	throw_runtime( "cannot join {0} and {1}", t1, t2 );
}

////////////////////////////////////////

std::shared_ptr<expr> environment::infer( const function &f, const std::vector<type_operator> &arg_types )
{
	precondition( arg_types.size() == f.args().size(), "mismatch for argument types" );

	std::shared_ptr<expr> result = f.result()->clone();

	_env.clear();
	for ( size_t i = 0; i < arg_types.size(); ++i )
		_env[f.args()[i]] = arg_types[i];

	visit( result );
	unify( result );
	return result;
}

////////////////////////////////////////

}

