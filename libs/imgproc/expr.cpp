
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

void func::write( std::ostream &out ) const
{
	out << _name << '(';
	std::copy( _args.begin(), _args.end(), std::ostream_iterator<std::u32string>( out, "," ) );
	out << ")\n{\n\t";
	_result->write( out );
	out << "\n}\n";
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

void aggregate_expr::write( std::ostream &out ) const
{
	out << "( " << *_value << " )";
}

////////////////////////////////////////

void tuple_expr::write( std::ostream &out ) const
{
	out << "{ " << *_value << " }";
}

////////////////////////////////////////

void call_expr::write( std::ostream &out ) const
{
	out << "( " << *_value << " )";
}

////////////////////////////////////////

void feature_expr::write( std::ostream &out ) const
{
	out << '.' << _value;
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
		if ( !( _value->can_be_feature() && ( next->can_be_feature() || next->is_call() ) ) )
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
		out << ", ";
		_next->write( out );
	}
}

////////////////////////////////////////

}

