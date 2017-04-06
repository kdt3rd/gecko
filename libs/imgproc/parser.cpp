//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <sstream>
#include <memory>
#include <base/contract.h>
#include <base/scope_guard.h>

#include "operators.h"
#include "parser.h"
#include "expr_parser.h"

////////////////////////////////////////

namespace imgproc
{

////////////////////////////////////////

parser::parser( std::map<std::u32string,std::shared_ptr<function>> &funcs, std::istream &in, utf::mode m )
	: _token( in, m ), _funcs( funcs )
{
	next_token();
}

////////////////////////////////////////

parser::parser( std::map<std::u32string,std::shared_ptr<function>> &funcs, const iterator &tok )
	: _token( tok ), _funcs( funcs )
{
	next_token();
}

////////////////////////////////////////

void parser::parse( void )
{
	while ( _token )
	{
		try
		{
			std::shared_ptr<function> f( get_function().release() );
			if ( f )
				_funcs[f->name()] = f;
		}
		catch ( std::exception &e )
		{
			std::stringstream msg;
			msg << "at " << _token.start_location() << '\n';
			base::print_exception( msg, e );
			add_error( msg.str() );
			next_token();

			// Search for "function" keyword
			while ( _token && _token.type() != TOK_FUNCTION && _token.type() != TOK_PUBLIC )
				next_token();
		}
	}
}

////////////////////////////////////////

bool parser::expect( const std::u32string &c )
{
	bool okay = false;
	if ( _token.type() == TOK_OPERATOR )
		okay = _token.split( c );
	else
		okay = ( _token.value() == c );

	if ( okay )
		next_token();

	return okay;
}

////////////////////////////////////////

bool parser::expect( token_type t )
{
	bool okay = ( _token.type() == t );
	if ( okay )
		next_token();
	return okay;
}

////////////////////////////////////////

bool parser::is_loop_modifier( const std::u32string &id )
{
	static std::set<std::u32string> loopmods =
	{
		U"sum",
		U"avg",
		U"min",
		U"max",
		U"count"
	};

	return loopmods.find( id ) != loopmods.end();
}

////////////////////////////////////////

void parser::next_token( void )
{
	_previous_end = _token.end_location();
	do
	{
		_token.next();
	} while ( _token.type() == TOK_COMMENT );
}

////////////////////////////////////////

void parser::add_error( const std::string &msg, const location &l )
{
	_has_errors = true;
	_messages.emplace_back( MSG_ERROR, l, msg );
}

////////////////////////////////////////

void parser::add_error( const std::string &msg )
{
	_messages.emplace_back( MSG_ERROR, previous_end(), msg );
}

////////////////////////////////////////

void parser::add_warning( const std::string &msg, const location &l )
{
	_messages.emplace_back( MSG_WARNING, l, msg );
}

////////////////////////////////////////

void parser::add_warning( const std::string &msg )
{
	_messages.emplace_back( MSG_WARNING, previous_end(), msg );
}

////////////////////////////////////////

void parser::add_info( const std::string &msg, const location &l )
{
	_messages.emplace_back( MSG_INFO, l, msg );
}

////////////////////////////////////////

void parser::add_info( const std::string &msg )
{
	_messages.emplace_back( MSG_INFO, previous_end(), msg );
}

////////////////////////////////////////

void parser::comments( void )
{
	while ( _token.type() == TOK_COMMENT )
	{
		_comments.push_back( std::move( _token.value() ) );
		next_token();
	}
}

////////////////////////////////////////

std::shared_ptr<expr> parser::expression( void )
{
	if ( _token.type() == TOK_FOR || _token.type() == TOK_IF || _token.type() == TOK_BLOCK_START || _token.type() == TOK_LAMBDA )
		return primary_expr();

	expr_parser eparser( _token, [&]() { return this->primary_expr(); } );
	auto result = eparser.expression();
	return result;
}

////////////////////////////////////////

std::shared_ptr<expr> parser::primary_expr( void )
{
	auto result = std::make_shared<expr>();

	if ( _token.type() == TOK_INTEGER )
	{
		result->set<integer_expr>( _token.value(), _token.integer() );
		next_token();
	}
	else if ( _token.type() == TOK_FLOATING )
	{
		result->set<floating_expr>( _token.value(), _token.floating() );
		next_token();
	}
	else if ( _token.type() == TOK_IDENTIFIER )
	{
		identifier_expr id( _token.value() );
		next_token();
		if ( _token.type() == TOK_PAREN_START )
		{
			result->set<call_expr>( id.value(), arguments() );
		}
		else if ( _token.type() == TOK_OPERATOR && _token.value()[0] == '=' )
		{
			std::u32string op = _token.value();
			while ( !op.empty() && operators.find( op ) == operators.end() )
				op.pop_back();

			if ( op.empty() )
			{
				next_token();
				if ( !_parsing_block )
					throw_runtime( "assignments can only be in blocks" );
				if ( _parsing_assign )
					throw_runtime( "double assignment" );
				_parsing_assign = true;
				on_scope_exit { _parsing_assign = false; };
				bool prev_block = _parsing_block;
				_parsing_block = false;
				on_scope_exit { _parsing_block = prev_block; };
				result->set<assign_expr>( id.value(), expression() );
			}
			else
				result->set<identifier_expr>( id );
		}
		else
			result->set<identifier_expr>( id );
	}
	else if ( _token.type() == TOK_PAREN_START )
	{
		bool prev_block = _parsing_block;
		_parsing_block = false;
		on_scope_exit { _parsing_block = prev_block; };
		next_token();
		std::shared_ptr<expr> e = expression();
		if ( e )
			result = e;
		if ( expect( TOK_PAREN_END ) )
		{
			if ( !e )
				throw_runtime( "empty parentheses" );
		}
		else
			throw_runtime( "missing ')' to end expression" );
	}
	else if ( _token.type() == TOK_MOD_START )
	{
		next_token();
		if ( _token.type() == TOK_IDENTIFIER )
		{
			std::u32string mod = _token.value();
			next_token();
			if ( !expect( TOK_MOD_END ) )
				throw_runtime( "expected ']' to end modifier, got '{0}'", _token.value() );
			if ( _token.type() == TOK_FOR )
			{
				result = loop_expr();
				result->get<for_expr>().add_modifier( mod );
			}
			else
				throw_runtime( "expected loop after modifier, got '{0}'", _token.value() );
		}
		else
			throw_runtime( "expected identifier for modifier, got '{0}'", _token.value() );
	}
	else if ( _token.type() == TOK_BLOCK_START )
		result = expr_block();
	else if ( _token.type() == TOK_IF )
	{
		bool prev_block = _parsing_block;
		_parsing_block = false;
		on_scope_exit { _parsing_block = prev_block; };
		result = if_expr();
	}
	else if ( _token.type() == TOK_FOR )
	{
		bool prev_block = _parsing_block;
		_parsing_block = false;
		on_scope_exit { _parsing_block = prev_block; };
		result = loop_expr();
	}
	else if ( _token.type() == TOK_LAMBDA )
	{
		auto f = std::shared_ptr<function>( get_function().release() );
		result = std::make_shared<expr>( lambda_expr( f ) );
	}

	return result;
}

////////////////////////////////////////

std::vector<std::shared_ptr<expr>> parser::arguments( void )
{
	if ( !expect( TOK_PAREN_START ) )
		throw_runtime( "expected '(', got '{0}'", _token.value() );

	std::vector<std::shared_ptr<expr>> list;

	// Expression is empty
	if ( _token.type() == TOK_PAREN_END )
		return std::vector<std::shared_ptr<expr>>();

	list.emplace_back( expression() );
	while ( expect( TOK_COMMA ) )
		list.emplace_back( expression() );

	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')' to finish arguments, got '{0}'", _token.value() );

	return list;
}

////////////////////////////////////////

std::shared_ptr<expr> parser::expr_block( void )
{
	if ( !expect( TOK_BLOCK_START ) )
		throw_runtime( "expected '{' to begin block" );

	_parsing_block = true;
	on_scope_exit { _parsing_block = false; };

	std::vector<std::shared_ptr<expr>> list;
	do
	{
		auto e = expression();
		if ( !e )
			throw_runtime( "oops: {0}", _token );
		list.emplace_back( e );
		if ( _token.type() == TOK_EXPRESSION_END )
			next_token();
	} while ( !expect( TOK_BLOCK_END ) );

	if ( list.empty() )
		throw_runtime( "block with no expressions" );

	std::shared_ptr<expr> last = list.back();
	if ( last->is<assign_expr>() )
	{
		auto loc = _expr_locs[last];
		throw_runtime( "last expression in block cannot be an assignment (starting at {0})", loc.first );
	}
	list.pop_back();

	while ( !list.empty() )
	{
		auto &a = list.back();
		if ( !a->is<assign_expr>() )
		{
			auto loc = _expr_locs[a];
			throw_runtime( "expected assignment (starting at {0})", loc.first );
		}
		a->get<assign_expr>().set_next( last );
		last = a;
		list.pop_back();
	}

	return last;
}

////////////////////////////////////////

std::unique_ptr<function> parser::get_function( void )
{
//	bool ispub = false;
	if ( _token.type() == TOK_PUBLIC )
	{
		next_token();
//		ispub = true;
	}

	if ( _token.type() == TOK_FUNCTION )
	{
		next_token();
		if ( _token.type() != TOK_IDENTIFIER )
			throw_runtime( "expected function name, got '{0}'", _token.value() );
	}
	else if ( _token.type() != TOK_LAMBDA )
		throw_runtime( "expected `function', got '{0}'", _token.value() );

	std::unique_ptr<function> f;
	if ( _token.type() != TOK_LAMBDA )
		f.reset( new function( _token.value() ) );
	else
		f.reset( new function );
	next_token();

	if ( expect( TOK_PAREN_START ) )
		arg_list( [&f]( const std::u32string &a, const std::u32string &mod ) { f->add_arg( a, mod ); } );
	else
		throw_runtime( "expected '(' to begin function arguments" );

	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')' to end function arguments, got '{0}'", _token.value() );

	f->set_result( expr_block() );

	return f;
}

////////////////////////////////////////

void parser::id_list( const std::function<void(std::u32string &)> &cb )
{
	if ( _token.type() == TOK_IDENTIFIER )
	{
		cb( _token.value() );
		next_token();

		while ( _token.type() == TOK_COMMA )
		{
			next_token();
			if ( _token.type() == TOK_IDENTIFIER )
			{
				cb( _token.value() );
				next_token();
			}
			else
				throw_runtime( "expected identifier" );
		}
	}
}

////////////////////////////////////////

void parser::arg_list( const std::function<void(std::u32string &,std::u32string &)> &cb )
{
	if ( arg( cb ) )
	{
		while ( _token.type() == TOK_COMMA )
		{
			next_token();
			if ( !arg( cb ) )
				throw_runtime( "expected identifier for argument name, got '{0}'", _token.value() );
		}
	}
}

////////////////////////////////////////

bool parser::arg( const std::function<void(std::u32string &,std::u32string &)> &cb )
{
	std::u32string mod;
	if ( _token.type() == TOK_IDENTIFIER )
	{
		cb( _token.value(), mod );
		next_token();
		return true;
	}
	else if ( expect( TOK_MOD_START ) )
	{
		if ( _token.type() == TOK_IDENTIFIER )
		{
			mod = _token.value();
			next_token();
			if ( !expect( TOK_MOD_END ) )
				throw_runtime( "expected ']', got '{0}'", _token.value() );
		}
		else
			throw_runtime( "expected modifier, got '{0}'", _token.value() );

		if ( _token.type() == TOK_IDENTIFIER )
		{
			cb( _token.value(), mod );
			next_token();
		}
		else
			throw_runtime( "expected argument name, got '{0}'", _token.value() );
		return true;
	}

	return false;
}

////////////////////////////////////////

std::shared_ptr<expr> parser::if_expr( void )
{
	if ( _token.type() != TOK_IF )
		throw_runtime( "expected `if', got '{0}'", _token.value() );
	next_token();

	if ( !expect( TOK_PAREN_START ) )
		throw_runtime( "expected '(' to start 'if' condition, got '{0}'", _token.value() );
	auto cond = expression();
	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')' to end 'if' condition, got '{0}'", _token.value() );

	auto wtrue = expression();
	decltype(wtrue) wfalse;
	if ( _token.type() == TOK_ELSE )
	{
		next_token();
		wfalse = expression();
	}

	imgproc::if_expr result( cond, wtrue, wfalse );
	return std::make_shared<expr>( std::move( result ) );
}

////////////////////////////////////////

std::shared_ptr<expr> parser::loop_range( void )
{
	std::shared_ptr<expr> start, end, by;
	start = expression();
	if ( expect( TOK_TO ) )
	{
		end = expression();
		if ( expect( TOK_BY ) )
			by = expression();
	}

	return std::make_shared<expr>( range_expr( start, end, by ) );
}

////////////////////////////////////////

std::shared_ptr<expr> parser::loop_expr( void )
{
	if ( _token.type() != TOK_FOR )
		throw_runtime( "expected `for'" );
	next_token();

	for_expr result;

	if ( !expect( TOK_PAREN_START ) )
		throw_runtime( "expected `(' after 'for', got '{0}'", _token.value() );

	std::vector<std::u32string> vars;
	id_list( [&vars]( const std::u32string &a ) { vars.push_back( a ); } );

	if ( vars.empty() )
		throw_runtime( "for loop variable(s) missing" );

	if ( !expect( TOK_SEPARATOR ) )
		throw_runtime( "expected ':', got '{0}'", _token.value() );

	do
	{
		_parsing_range = true;
		on_scope_exit { _parsing_range = false; };
		result.add_range( std::move( loop_range()->get<range_expr>() ) );
	} while ( expect( TOK_COMMA ) );

	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')', got '{0}'", _token.value() );

	if ( result.ranges().empty() )
		throw_runtime( "no range(s) specified for loop" );

	if ( result.ranges().size() > 1 )
	{
		if ( vars.size() != result.ranges().size() )
			throw_runtime( "expected {0} ranges, got {1}", vars.size(), result.ranges().size() );
	}
	else
	{
		while ( result.ranges().size() < vars.size() )
			result.add_range( range_expr( result.ranges().back() ) );
	}

	result.set_variable_names( std::move( vars ) );

	bool oldassign = _parsing_assign;
	_parsing_assign = false;
	on_scope_exit { _parsing_assign = oldassign; };
	result.set_result( expression() );

	return std::make_shared<expr>( std::move( result ) );
}

////////////////////////////////////////

}

