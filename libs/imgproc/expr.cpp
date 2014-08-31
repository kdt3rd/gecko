
#include "expr.h"
#include <iostream>
#include <utf/utf.h>
#include <iterator>
#include <memory>
#include <base/scope_guard.h>
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

std::string prefix_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	return base::format( "({0}{1})", operation(), expression()->compile( code, scope ) );
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

std::string postfix_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	return base::format( "({0}{1})", expression()->compile( code, scope ), operation() );
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

std::string infix_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	std::string e1 = expression1()->compile( code, scope );
	std::string e2 = expression2()->compile( code, scope );
	return base::format( "({0} {1} {2})", e1, operation(), e2 );
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

std::string circumfix_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	throw_not_yet();
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

std::string postcircumfix_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
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

std::string error_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	throw_not_yet();
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

std::string number_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	if ( code.expected().first != data_type::UNKNOWN )
		return base::format( "{0}({1})", cpp_type( code.expected() ), value() );
	else
		return base::format( "{0}", value() );
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

std::string identifier_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	const std::u32string &v = value();
	if ( code.has_range_modifier() )
		return code.range( value() );
	if ( scope->get( v ).get_type() != code.expected() && code.expected().first != data_type::UNKNOWN )
		return base::format( "{0}({1})", cpp_type( code.expected() ), value() );
	else
		return base::format( "{0}", value() );
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
	if ( _next )
		_next->write( out );
}

////////////////////////////////////////

type assign_expr::result_type( std::shared_ptr<scope> &sc ) const
{
	auto t = _expr->result_type( sc );
	sc->add( _var, t );
	return _next->result_type( sc );
}

////////////////////////////////////////

std::string assign_expr::compile( compile_context &code, std::shared_ptr<scope> &sc ) const
{
	std::string e = _expr->compile( code, sc );
	auto t = _expr->result_type( sc );
	code.line( "{0} {1} = {2};", cpp_type( t ), _var, e );
	sc->add( _var, t );
	return _next->compile( code, sc );
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

	if ( sc->functions()->has( _func ) )
		return sc->functions()->compile( _func, args );
	else
	{
		auto t = sc->get( _func );
		return { t.get_type().first, 0 };
	}
}

////////////////////////////////////////

std::string call_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
{
	std::stringstream str;
	str << function() << "( ";
	const auto &args = arguments();
	for ( size_t i = 0; i < args.size(); ++i )
	{
		if ( i > 0 )
			str << ", ";
		str << args[i]->compile( code, scope );
	}
	str << " )";
	return str.str();
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

std::string chain_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
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

std::string arguments_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
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
	// Process the range, make sure they are of the correct type
	for ( auto r: _ranges )
	{
//		auto t = r->result_type( sc );
//		if ( t.second != 0 )
//			throw_runtime( "range of invalid type ({0})", t );
	}

	auto newsc = std::make_shared<scope>( sc );
	for ( auto v: _vars )
		newsc->add( v, { data_type::UINT64, 0 } );

	auto t = _result->result_type( newsc );

	if ( _mod.empty() )
		return { t.first, _vars.size() };
	else if ( _mod == U"count" )
		return { data_type::UINT64, 0 };
	else if ( _mod == U"sum" )
		return t;
	else
		throw_not_yet();
}

////////////////////////////////////////

std::string for_expr::compile( compile_context &code, std::shared_ptr<scope> &sc ) const
{
	if ( _mod == U"count" )
	{
		code.line( "int64_t _count = 0;" );
		auto newsc = std::make_shared<scope>( sc );

		for ( int i = _vars.size() - 1; i >= 0; --i )
		{
			std::string exp = "for ( " + _ranges[i]->compile( code, newsc ) + " )";
			code.line( exp, _vars[i] );
			code.line( "{" );
			code.indent_more();
			newsc->add( _vars[i], { data_type::UINT64, 0 } );
		}

		code.line( "_count += {0};", result()->compile( code, newsc ) );

		for ( size_t i = 0; i < _vars.size(); ++i )
		{
			code.indent_less();
			code.line( "}" );
		}
		return "_count";
	}
	else if ( _mod == U"sum" )
	{
		auto tmpsc = std::make_shared<scope>( sc );
		for ( auto v: _vars )
			tmpsc->add( v, { data_type::UINT64, 0 } );

		auto t = _result->result_type( tmpsc );

		code.line( "{0} _sum = 0;", cpp_type( t ) );

		auto newsc = std::make_shared<scope>( sc );
		for ( int i = _vars.size() - 1; i >= 0; --i )
		{
			std::string exp = "for ( " + _ranges[i]->compile( code, newsc ) + " )";
			code.line( exp, _vars[i] );
			code.line( "{" );
			code.indent_more();
			newsc->add( _vars[i], { data_type::UINT64, 0 } );
		}

		code.line( "_sum += {0};", result()->compile( code, newsc ) );

		for ( size_t i = 0; i < _vars.size(); ++i )
		{
			code.indent_less();
			code.line( "}" );
		}
		return "_sum";
	}
	else if ( _mod == U"max" )
	{
		throw_not_yet();
	}
	else if ( _mod == U"min" )
	{
		throw_not_yet();
	}
	else
	{
		auto t = result_type( sc );
		std::stringstream sizes;
		std::stringstream offsets;
		for ( size_t i = 0; i < t.second; ++i )
		{
			if ( i > 0 )
				sizes << ", ";
			code.line( "int64_t _size{0} = {1};", i, _ranges[i]->get_size( code, sc ) );
			sizes << "_size" << i;
			code.line( "int64_t _offset{0} = {1};", i, _ranges[i]->get_offset( code, sc ) );
			offsets << "_offset" << i;
		}
		if ( sizes.str().empty() )
			code.line( "{0} _result;", cpp_type( t ), sizes.str() );
		else
		{
			code.line( "{0} _result( {1} );", cpp_type( t ), sizes.str() );
			code.line( "_result.set_offset( {0} );", offsets.str() );
		}
		auto newsc = std::make_shared<scope>( sc );

		std::stringstream vars;
		for ( size_t i = _vars.size(); i > 0; --i )
		{
			if ( i != _vars.size() )
				vars << ", ";
			vars << _vars[i-1];
			std::string exp = "for ( " + _ranges[i-1]->compile( code, newsc ) + " )";
			code.line( exp, _vars[i-1] );
			code.line( "{" );
			code.indent_more();
			newsc->add( _vars[i-1], { data_type::UINT64, 0 } );
		}

		code.line( "_result( {0} ) = {1};", vars.str(), result()->compile( code, newsc ) );

		for ( size_t i = 0; i < _vars.size(); ++i )
		{
			code.indent_less();
			code.line( "}" );
		}
		return "_result";
	}
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
	auto t = _true->result_type( scope );
	if ( _false )
	{
		auto o = _false->result_type( scope );
		logic_check( t == o, "if/else type mismatch" );
	}
	return t;
}

////////////////////////////////////////

std::string if_expr::compile( compile_context &code, std::shared_ptr<scope> &scope ) const
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
	return { data_type::INT64, 0 };
	/*
	auto t = _start->result_type( scope );
//	if ( t.second != 0 )
//	{
//		std::stringstream msg;
//		_start->write( msg );
//		throw_runtime( "got array {0} for range expression ({1})", t, msg.str() );
//	}
	if ( _end )
	{
		_end->result_type( scope );
//		if ( t.second != 0 )
//		{
//			std::stringstream msg;
//			_end->write( msg );
//			throw_runtime( "got array {0} for range end expression ({1})", t, msg.str() );
//		}
	}

	if ( _by )
	{
		_end->result_type( scope );
//		if ( t.second != 0 )
//		{
//			std::stringstream msg;
//			_by->write( msg );
//			throw_runtime( "got array {0} for range by expression ({1})", t, msg.str() );
//		}
	}
	return t;
	*/
}

////////////////////////////////////////

std::string range_expr::compile( compile_context &code, std::shared_ptr<scope> &sc ) const
{
	std::stringstream tmp;
	tmp << "int64_t {0} = ";
	if ( _end )
	{
		code.set_lower_range( _index );
		on_scope_exit { code.clear_range_index(); };
   		tmp << _start->compile( code, sc ) << ';';

		code.set_upper_range( _index );
		on_scope_exit { code.clear_range_index(); };
		tmp << " {0} < " << _end->compile( code, sc ) << ';';
	}
	else
	{
		code.set_lower_range( _index );
		on_scope_exit { code.clear_range_index(); };
		tmp << _start->compile( code, sc ) << ';';

		code.set_upper_range( _index );
		on_scope_exit { code.clear_range_index(); };
		tmp << " {0} < " << _start->compile( code, sc ) << ';';
	}
	tmp << " {0} += ";
	if ( _by )
		tmp << _by->compile( code, sc );
	else
		tmp << "1";
	return std::move( tmp.str() );
}

////////////////////////////////////////

std::string range_expr::get_size( compile_context &code, std::shared_ptr<scope> &sc ) const
{
	std::stringstream tmp;
	tmp << "static_cast<int64_t>( ";
	if ( _end )
	{
		code.set_upper_range( _index );
		on_scope_exit { code.clear_range_index(); };
		tmp << '(' << _end->compile( code, sc ) << " - ";

		code.set_lower_range( _index );
		on_scope_exit { code.clear_range_index(); };
	   	tmp << _start->compile( code, sc ) << ')';
	}
	else
	{
		code.set_upper_range( _index );
		on_scope_exit { code.clear_range_index(); };
		tmp << '(' << _start->compile( code, sc ) << ')';
	}
	tmp << " )";
	return tmp.str();
}

////////////////////////////////////////

std::string range_expr::get_offset( compile_context &code, std::shared_ptr<scope> &sc ) const
{
	std::stringstream tmp;
	code.set_lower_range( _index );
	on_scope_exit { code.clear_range_index(); };
	tmp << "static_cast<int64_t>(";
	tmp << _start->compile( code, sc ) << ')';
	return tmp.str();
}

////////////////////////////////////////

}

