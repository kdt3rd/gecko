
#include "expr.h"
#include "function.h"
#include <utf/utf.h>
#include <iterator>

////////////////////////////////////////

namespace
{
using namespace imgproc;

class printer
{
public:
	printer( std::ostream &out )
		: _out( out )
	{
	}

	void operator()( const prefix_expr &e )
	{
		_out << '(' << e.operation() << e.expression() << ')';
	}

	void operator()( const postfix_expr &e )
	{
		_out << '(' << e.expression() << e.operation() << ')';
	}

	void operator()( const infix_expr &e )
	{
		_out << '(' << e.expression1() << e.operation() << e.expression2() << ')';
	}

	void operator()( const circumfix_expr &e )
	{
		_out << e.open() << e.expression() << e.close();
	}

	void operator()( const postcircumfix_expr &e )
	{
		_out << '(' << e.expression1() << e.open() << e.expression2() << e.close() << ')';
	}

	void operator()( const integer_expr &e )
	{
		_out << e.value();
	}

	void operator()( const floating_expr &e )
	{
		_out << e.value();
	}

	void operator()( const identifier_expr &e )
	{
		_out << e.value();
	}

	void operator()( const call_expr &e )
	{
		_out << e.function() << '(';
		auto &args = e.arguments();
		std::copy( args.begin(), args.end(), base::infix_ostream_iterator<std::shared_ptr<expr>>( _out, ", " ) );
		_out << ')';
	}

	void operator()( const if_expr &e )
	{
		_out << "if(" << e.condition() << ") " << e.when_true() << " else " << e.when_false();
	}

	void operator()( const range_expr &e )
	{
		_out << e.start();
		if ( e.end() )
			_out << " to " << e.end();
		if ( e.by() )
			_out << " by " << e.by();
	}

	void operator()( const for_expr &e )
	{
		auto &mods = e.modifiers();
		if ( !mods.empty() )
		{
			_out << "[ ";
			std::copy( mods.begin(), mods.end(), base::infix_ostream_iterator<std::u32string>( _out, ", " ) );
			_out << " ] ";
		}

		_out << "for ( ";
		auto &vars = e.variables();
		std::copy( vars.begin(), vars.end(), base::infix_ostream_iterator<std::u32string>( _out, ", " ) );

		_out << ": ";
		auto &ranges = e.ranges();
		for ( size_t i = 1; i < ranges.size(); ++i )
		{
			this->operator()( ranges[i-1] );
			_out << ", ";
		}
		if ( !ranges.empty() )
			this->operator()( ranges.back() );
		_out << " ) ";
		_out << e.result();
	}

	void operator()( const assign_expr &e )
	{
		_out << e.variable() << " = " << e.expression();
		if ( e.next() )
			_out << "; " << e.next();
	}

	void operator()( const lambda_expr &e )
	{
		_out << ( *e.get_function() );
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	type operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for printer" );
		return type();
	}

private:
	std::ostream &_out;
};

////////////////////////////////////////

class cloner
{
public:
	std::shared_ptr<expr> operator()( const prefix_expr &e )
	{
		return std::make_shared<expr>( prefix_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const postfix_expr &e )
	{
		return std::make_shared<expr>( postfix_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const infix_expr &e )
	{
		return std::make_shared<expr>( infix_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const circumfix_expr &e )
	{
		return std::make_shared<expr>( circumfix_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const postcircumfix_expr &e )
	{
		return std::make_shared<expr>( postcircumfix_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const integer_expr &e )
	{
		return std::make_shared<expr>( integer_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const floating_expr &e )
	{
		return std::make_shared<expr>( floating_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const identifier_expr &e )
	{
		return std::make_shared<expr>( identifier_expr( e ) );
	}

	std::shared_ptr<expr> operator()( const call_expr &e )
	{
		std::vector<std::shared_ptr<expr>> args;
		for ( auto &a: e.arguments() )
			args.push_back( a->clone() );
		return std::make_shared<expr>( call_expr( e.function(), std::move( args ) ) );
	}

	std::shared_ptr<expr> operator()( const if_expr &e )
	{
		return std::make_shared<expr>( if_expr( e.condition()->clone(), e.when_true()->clone(), e.when_false()->clone() ) );
	}

	std::shared_ptr<expr> operator()( const range_expr &e )
	{
		throw_not_yet();
	}

	std::shared_ptr<expr> operator()( const for_expr &e )
	{
		return std::make_shared<expr>( for_expr( e.modifiers(), e.variables(), e.ranges(), e.result()->clone() ) );
	}

	std::shared_ptr<expr> operator()( const assign_expr &e )
	{
		return std::make_shared<expr>( assign_expr( e.variable(), e.expression()->clone(), e.next()->clone() ) );
	}

	std::shared_ptr<expr> operator()( const lambda_expr &e )
	{
		return std::make_shared<expr>( lambda_expr( e.get_function()->clone() ) );
	}

	// Compile-time check for any missing operator() implementation
	template<typename T>
	std::shared_ptr<expr> operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for printer" );
		return std::shared_ptr<expr>();
	}
};

}

////////////////////////////////////////

namespace imgproc
{

////////////////////////////////////////

std::shared_ptr<expr> expr::clone( void )
{
	cloner c;
	return base::visit<std::shared_ptr<expr>>( c, *this );
}


////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const std::shared_ptr<expr> &e )
{
	printer p( out );
	base::visit( p, *e );
	if ( e->get_type().valid() )
		out << '@' << e->get_type() << '@';
	return out;
}

////////////////////////////////////////

}

