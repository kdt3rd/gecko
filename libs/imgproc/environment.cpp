
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
		throw_not_yet();
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

std::shared_ptr<expr> infer( const function &f, const std::vector<type_operator> &arg_types )
{
	precondition( arg_types.size() == f.args().size(), "mismatch for argument types" );

	std::shared_ptr<expr> result = f.result()->clone();

	environment env;
	for ( size_t i = 0; i < arg_types.size(); ++i )
		env[f.args()[i]] = arg_types[i];

	env.visit( result );
	env.unify( result );

	return result;
}

////////////////////////////////////////

}

