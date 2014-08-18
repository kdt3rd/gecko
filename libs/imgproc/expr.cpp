
#include "expr.h"
#include <iostream>
#include <utf/utf.h>
#include <iterator>

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

void string_expr::write( std::ostream &out ) const
{
	out << '"' << _value << '"';
}

////////////////////////////////////////

void number_expr::write( std::ostream &out ) const
{
	out << _value;
}

////////////////////////////////////////

void char_expr::write( std::ostream &out ) const
{
	out << '\'' << _value << '\'';
}

////////////////////////////////////////

void identifier_expr::write( std::ostream &out ) const
{
	out << _value;
}

////////////////////////////////////////

void operator_expr::write( std::ostream &out ) const
{
	out << _value;
}

////////////////////////////////////////

void assign_expr::write( std::ostream &out ) const
{
	out << _var << " = ";
	_expr->write( out );
}

////////////////////////////////////////

void tuple_expr::write( std::ostream &out ) const
{
	out << "{ " << *_value << " }";
}

////////////////////////////////////////

void call_expr::write( std::ostream &out ) const
{
	out << *_func << "( " << *_args << " )";
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

		// Check for "obj.f" or "f(...)" and skip the space
		if ( !next->is_call() )
			out << ' ';
		_next->write( out );
	}
}

////////////////////////////////////////

void list_expr::write( std::ostream &out ) const
{
	value()->write( out );
	if ( _next )
	{
		out << "; ";
		_next->write( out );
	}
}

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

void for_expr::write( std::ostream &out ) const
{
	out << "for ( ";

	std::copy( _vars.begin(), _vars.end() - 1, std::ostream_iterator<std::u32string>( out, ", " ) );
	out << _vars.back() << ": ";

	std::copy( _ranges.begin(), _ranges.end() - 1, std::ostream_iterator<std::shared_ptr<expr>>( out, ", " ) );
	out << *(_ranges.back()) << " ) ";

	_result->write( out );
}
////////////////////////////////////////

}

