
#include "expr.h"
#include <iostream>
#include <utf/utf.h>
#include <iterator>
#include <memory>
#include "scope.h"

namespace imgproc
{

////////////////////////////////////////

expr::~expr( void )
{
}

////////////////////////////////////////

prefix_expr::prefix_expr( const std::u32string &op, const std::shared_ptr<expr> &x )
	: _op( op ), _x( x )
{
}

////////////////////////////////////////

void prefix_expr::write( std::ostream &out ) const
{
	out << _op << '(';
	_x->write( out );
	out << ')';
}

////////////////////////////////////////

type prefix_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	return _x->result_type( scope );
}

////////////////////////////////////////

postfix_expr::postfix_expr( const std::u32string &op, const std::shared_ptr<expr> &x )
	: _op( op ), _x( x )
{
}

////////////////////////////////////////

void postfix_expr::write( std::ostream &out ) const
{
	out << '(';
	_x->write( out );
	out << ')' << _op;
}

////////////////////////////////////////

type postfix_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	return _x->result_type( scope );
}

////////////////////////////////////////

infix_expr::infix_expr( const std::u32string &op, const std::shared_ptr<expr> &x, const std::shared_ptr<expr> &y )
	: _op( op ), _x( x ), _y( y )
{
}

////////////////////////////////////////

void infix_expr::write( std::ostream &out ) const
{
	out << '(';
	_x->write( out );
	out << ')' << _op << '(';
	_y->write( out );
	out << ')';
}

////////////////////////////////////////

type infix_expr::result_type( std::shared_ptr<scope> &scope ) const
{
//	auto a = _x->result_type( scope );
	auto b = _y->result_type( scope );
//	logic_check( a == b, "using operator on two different types ({0} and {1})", a, b );	
	return b;
}

////////////////////////////////////////

circumfix_expr::circumfix_expr( const std::u32string &op, const std::u32string &cl, const std::shared_ptr<expr> &x )
	: _open( op ), _close( cl ), _x( x )
{
}

////////////////////////////////////////

void circumfix_expr::write( std::ostream &out ) const
{
	out << _open;
	_x->write( out );
	out << _close;
}

////////////////////////////////////////

type circumfix_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	return _x->result_type( scope );
}

////////////////////////////////////////

postcircumfix_expr::postcircumfix_expr( const std::u32string &op, const std::u32string &cl, const std::shared_ptr<expr> &x, const std::shared_ptr<expr> &y )
	: _open( op ), _close( cl ), _x( x ), _y( y )
{
}

////////////////////////////////////////

void postcircumfix_expr::write( std::ostream &out ) const
{
	_x->write( out );
	out << _open;
	_y->write( out );
	out << _close;
}

////////////////////////////////////////

type postcircumfix_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	throw_not_yet();
}

////////////////////////////////////////

void func::write( std::ostream &out ) const
{
	out << "function " << _name << "( ";
	if ( !_args.empty() )
	{
		std::copy( _args.begin(), _args.end()-1, std::ostream_iterator<std::u32string>( out, ", " ) );
		out << _args.back();
	}
	out << " )\n{\n    ";
	_result->write( out );
	out << ";\n}\n";
}

////////////////////////////////////////

value_expr::~value_expr( void )
{
}

////////////////////////////////////////

error_expr::~error_expr( void )
{
}

////////////////////////////////////////

void error_expr::write( std::ostream &out ) const
{
	out << "/* " << _msg << " */";
}


////////////////////////////////////////

type error_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	throw_runtime( "error expression has no type" );
}

////////////////////////////////////////

void number_expr::write( std::ostream &out ) const
{
	out << _value;
}

////////////////////////////////////////

type number_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	return { data_type::FLOAT32, 0 };
}

////////////////////////////////////////

void identifier_expr::write( std::ostream &out ) const
{
	out << _value;
}

////////////////////////////////////////

type identifier_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	return scope->get( _value ).get_type();
}

////////////////////////////////////////

/*
void operator_expr::write( std::ostream &out ) const
{
	out << _value;
}
*/

////////////////////////////////////////

void assign_expr::write( std::ostream &out ) const
{
	out << _var << " = ";
	_expr->write( out );
	out << "; ";
	_next->write( out );
}

////////////////////////////////////////

type assign_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	auto t = _next->result_type( scope );
	scope->add( _var, t );
	return t;
}

////////////////////////////////////////

/*
void tuple_expr::write( std::ostream &out ) const
{
	out << "{ " << *_value << " }";
}
*/

////////////////////////////////////////

void call_expr::write( std::ostream &out ) const
{
	out << _func << "( ";
	if ( !_args.empty() )
	{
		_args.front()->write( out );
		for ( size_t i = 1 ; i < _args.size(); ++i )
		{
			out << ", ";
			_args[i]->write( out );
		}
	}
	out << " )";
}

////////////////////////////////////////

type call_expr::result_type( std::shared_ptr<scope> &sc ) const
{
	std::vector<type> args;
	for ( size_t i = 0; i < _args.size(); ++i )
		args.push_back( _args[i]->result_type( sc ) );

	return sc->functions()->compile( _func, args );
}

////////////////////////////////////////

void chain_expr::write( std::ostream &out ) const
{
	_value->write( out );
	if ( _next )
	{
		std::shared_ptr<expr> next;
		{
			auto cnext = std::dynamic_pointer_cast<chain_expr>( _next );
			if ( cnext )
				next = cnext->value();
			else
				next = _next;
		}

		_next->write( out );
	}
}

////////////////////////////////////////

type chain_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	throw_not_yet();
}

////////////////////////////////////////

/*
void list_expr::write( std::ostream &out ) const
{
	value()->write( out );
	if ( _next )
	{
		out << "; ";
		_next->write( out );
	}
}
*/

////////////////////////////////////////

void arguments_expr::write( std::ostream &out ) const
{
	value()->write( out );
	if ( _next )
	{
		out << ", ";
		_next->write( out );
	}
}

////////////////////////////////////////

type arguments_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	throw_not_yet();
}

////////////////////////////////////////

void block_expr::write( std::ostream &out ) const
{
	out << "{ ";
	value()->write( out );
	out << "; }";
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const std::shared_ptr<expr> &e )
{
	out << *e;
	return out;
}

////////////////////////////////////////

void for_expr::write( std::ostream &out ) const
{
	if ( !_mod.empty() )
		out << _mod << ' ';
	out << "for ( ";

	std::copy( _vars.begin(), _vars.end() - 1, std::ostream_iterator<std::u32string>( out, ", " ) );
	out << _vars.back() << ": ";

	std::copy( _ranges.begin(), _ranges.end() - 1, std::ostream_iterator<std::shared_ptr<expr>>( out, ", " ) );
	out << *(_ranges.back()) << " ) ";

	_result->write( out );
}

////////////////////////////////////////

type for_expr::result_type( std::shared_ptr<scope> &sc ) const
{
	if ( _mod.empty() )
	{
		auto newsc = std::make_shared<scope>( sc );
		for ( size_t i = 0; i < _vars.size(); ++i )
			newsc->add( _vars[i], { data_type::UINT32, 0 } );

		auto t = _result->result_type( newsc );
		return { t.first, _vars.size() };
	}
	else if ( _mod == U"count" )
	{
		return { data_type::UINT32, 0 };
	}
	else if ( _mod == U"sum" )
	{
		return { data_type::FLOAT32, 0 };
	}
	else
		throw_not_yet();
}

////////////////////////////////////////

void if_expr::write( std::ostream &out ) const
{
	out << "if ( ";
	_condition->write( out );
	out << " ) ";
	_true->write( out );
	if ( _false )
	{
		out << " else ";
		_false->write( out );
	}
}

////////////////////////////////////////

type if_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	throw_not_yet();
}

////////////////////////////////////////

void range_expr::write( std::ostream &out ) const
{
	_start->write( out );
	if ( _end )
	{
		out << " to ";
		_end->write( out );

		if ( _by )
		{
			out << " by ";
			_by->write( out );
		}
	}
}

////////////////////////////////////////

type range_expr::result_type( std::shared_ptr<scope> &scope ) const
{
	throw_not_yet();
}

////////////////////////////////////////

}

