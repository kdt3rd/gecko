
#include <sstream>
#include <memory>
#include <base/contract.h>

#include "parser.h"

////////////////////////////////////////

namespace imgproc
{

////////////////////////////////////////

parser::parser( std::vector<std::shared_ptr<func>> &funcs, std::istream &in, utf::mode m )
	: _token( in, m ), _funcs( funcs )
{
	next_token();
}

////////////////////////////////////////

parser::parser( std::vector<std::shared_ptr<func>> &funcs, const iterator &tok )
	: _token( tok ), _funcs( funcs )
{
	next_token();
}

////////////////////////////////////////

void parser::operator()( void )
{
	while ( _token )
	{
		try
		{
			std::shared_ptr<func> f( function().release() );
			if ( f )
				_funcs.push_back( f );
		}
		catch ( std::exception &e )
		{
			std::stringstream msg;
			base::print_exception( msg, e );
			add_error( msg.str() );
			next_token();

			// Search for "function" keyword
			while ( _token && _token.type() != TOK_FUNCTION )
				next_token();
		}
	}
}

////////////////////////////////////////

bool parser::expect( const std::u32string &c )
{
	bool okay = false;
	if ( _token.type() == TOK_SYMBOL )
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

void parser::next_token( void )
{
	_previous_end = _token.end_location();
	do
	{
		_token.next();
//		if( !_token.next() )
//			throw_runtime( "end of file" );
	} while ( _token.type() == TOK_COMMENT );
}

////////////////////////////////////////

void parser::add_error( const std::string &msg, const location &l )
{
	_messages.emplace_back( MSG_ERROR, l, msg );
}

////////////////////////////////////////

void parser::add_error( const std::string &msg )
{
	_messages.emplace_back( MSG_ERROR, start_location(), msg );
}

////////////////////////////////////////

void parser::add_warning( const std::string &msg, const location &l )
{
	_messages.emplace_back( MSG_WARNING, l, msg );
}

////////////////////////////////////////

void parser::add_warning( const std::string &msg )
{
	_messages.emplace_back( MSG_WARNING, start_location(), msg );
}

////////////////////////////////////////

void parser::add_info( const std::string &msg, const location &l )
{
	_messages.emplace_back( MSG_INFO, l, msg );
}

////////////////////////////////////////

void parser::add_info( const std::string &msg )
{
	_messages.emplace_back( MSG_INFO, start_location(), msg );
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
	std::vector<std::shared_ptr<expr>> result;

	bool done = false;
	while ( !done )
	{
		std::shared_ptr<expr> e;
		bool symbols = false;
		while ( _token.type() == TOK_SYMBOL )
		{
			symbols = true;
			result.emplace_back( new operator_expr( _token.value() ) );
			next_token();
		}

		// Expression should be separated by operators
		if ( !symbols && !result.empty() )
		{
			switch ( _token.type() )
			{
				case TOK_PAREN_END:
				case TOK_EXPRESSION_END:
				case TOK_BLOCK_END:
				case TOK_SEPARATOR:
				case TOK_COMMA:
					break;

				case TOK_IF:
				case TOK_ELSE:
				case TOK_FOR:
				case TOK_FUNCTION:
				case TOK_TO:
				case TOK_BY:
					break;

				default:
					std::cerr << *result.back() << std::endl;
					throw_runtime( "expressions should be separated at '{0}'", _token.value() );
			}
		}

		e = primary_expr();
		if ( e )
		{
			result.push_back( e );

			if ( _token.type() == TOK_EXPRESSION_END )
			{
				next_token();
				done = true;
			}
		}
		else
			done = true;
	}

	if ( result.empty() )
		throw_runtime( "expected expression, got '{0}'", _token.value() );
	else if ( result.size() == 1 )
		return result[0];
	else
		return std::shared_ptr<expr>( new chain_expr( result.begin(), result.end() ) );
}

////////////////////////////////////////

std::shared_ptr<expr> parser::primary_expr( void )
{
	std::shared_ptr<expr> result;

	if ( _token.type() == TOK_STRING )
	{
		result.reset( new string_expr( _token.value() ) );
		next_token();
	}
	else if ( _token.type() == TOK_NUMBER )
	{
		result.reset( new number_expr( _token.value() ) );
		next_token();
	}
	else if ( _token.type() == TOK_CHARACTER )
	{
		result.reset( new char_expr( _token.value() ) );
		next_token();
	}
	else if ( _token.type() == TOK_IDENTIFIER )
	{
		result.reset( new identifier_expr( _token.value() ) );
		next_token();
		if ( _token.type() == TOK_PAREN_START )
			result = std::make_shared<call_expr>( result, arguments() );
	}
	else if ( _token.type() == TOK_PAREN_START )
	{
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
	else if ( _token.type() == TOK_BLOCK_START )
	{
		next_token();
		std::vector<std::shared_ptr<expr>> list;
		while ( _token.type() != TOK_BLOCK_END )
			list.emplace_back( expression() );
		result = std::make_shared<block_expr>( list.begin(), list.end() );
		expect( TOK_BLOCK_END );
	}
	else if ( _token.type() == TOK_IF )
		result = if_expr();
	else if ( _token.type() == TOK_FOR )
			result = for_expr();

	return result;
}

////////////////////////////////////////

std::shared_ptr<expr> parser::arguments( void )
{
	if ( !expect( TOK_PAREN_START ) )
		throw_runtime( "expected '(', got '{0}'", _token.value() );

	std::vector<std::shared_ptr<expr>> list;

	// Expression is empty
	if ( _token.type() == TOK_PAREN_END )
		return std::shared_ptr<expr>();

	list.emplace_back( expression() );
	while ( expect( TOK_COMMA ) )
		list.emplace_back( expression() );

	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')', got '{0}'", _token.value() );

	return std::make_shared<arguments_expr>( list.begin(), list.end() );
}

////////////////////////////////////////

std::unique_ptr<func> parser::function( void )
{
	if ( _token.type() != TOK_FUNCTION )
		throw_runtime( "expected `function', got '{0}'", _token.value() );
	next_token();

	if ( _token.type() != TOK_IDENTIFIER )
		throw_runtime( "expected function name, got '{0}'", _token.value() );
	std::unique_ptr<func> f( new func( _token.value() ) );
	next_token();

	if ( expect( TOK_PAREN_START ) )
		id_list( [&f]( const std::u32string &a ) { f->add_arg( a ); } );
	else
		throw_runtime( "expected '(' to begin function arguments" );

	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')' to end function arguments, got '{0}'", _token.value() );

	if ( !expect( TOK_BLOCK_START ) )
		throw_runtime( "expected '{' to begin function, got '{0}'", _token.value() );

	std::vector<std::shared_ptr<expr>> list;
	while ( !expect( TOK_BLOCK_END ) )
		list.emplace_back( expression() );

	f->set_result( std::make_shared<list_expr>( list.begin(), list.end() ) );

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

std::shared_ptr<expr> parser::if_expr( void )
{
	if ( _token.type() != TOK_IF )
		throw_runtime( "expected `if', got '{0}'", _token.value() );
	next_token();

	auto result = std::make_shared<imgproc::if_expr>();

	if ( !expect( TOK_PAREN_START ) )
		throw_runtime( "expected '(' to start 'if' condition, got '{0}'", _token.value() );
	result->set_condition( expression() );
	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')' to end 'if' condition, got '{0}'", _token.value() );

	result->set_result( expression() );
	if ( _token.type() == TOK_ELSE )
	{
		next_token();
		result->set_else( expression() );
	}
	return result;
}

////////////////////////////////////////

std::shared_ptr<expr> parser::for_range( void )
{
	std::shared_ptr<expr> start, end, by;
	start = expression();
	if ( expect( TOK_TO ) )
	{
		end = expression();
		if ( expect( TOK_BY ) )
			by = expression();
	}

	return std::make_shared<range_expr>( start, end, by );
}

////////////////////////////////////////

std::shared_ptr<expr> parser::for_expr( void )
{
	if ( _token.type() != TOK_FOR )
		throw_runtime( "expected `for'" );
	next_token();

	auto result = std::make_shared<imgproc::for_expr>();

	if ( !expect( TOK_PAREN_START ) )
		throw_runtime( "expected `(' after 'for', got '{0}'", _token.value() );

	id_list( [&result]( const std::u32string &a ) { result->add_variable( a ); } );

	if ( !expect( TOK_SEPARATOR ) )
		throw_runtime( "expected ':', got '{0}'", _token.value() );

	while ( !expect( TOK_PAREN_END ) )
		result->add_range( for_range() );

	result->set_result( expression() );

	return result;
}

////////////////////////////////////////

}

