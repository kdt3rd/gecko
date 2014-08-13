
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
			std::cout << "parsing function" << std::endl;
			std::shared_ptr<func> f( function().release() );
			if ( f )
				_funcs.push_back( f );
			std::cout << "parsing done" << std::endl;
		}
		catch ( std::exception &e )
		{
			std::stringstream msg;
			base::print_exception( msg, e );
			add_error( msg.str() );
			// Search for "function" keyword
			while ( _token && _token.type() != TOK_KEYWORD && _token.value() != U"function" )
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
		if( !_token.next() )
			throw_runtime( "end of file" );
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
	while ( _token.type() == TOK_COMMENT || _token.type() == TOK_COMMENT_BLOCK )
	{
		_comments.push_back( std::move( _token.value() ) );
		next_token();
	}
}

////////////////////////////////////////

std::shared_ptr<expr> parser::expression( void )
{
	std::vector<std::shared_ptr<expr>> list;
	std::vector<std::shared_ptr<expr>> result;

	std::shared_ptr<expr> e;
	do
	{
		if ( _token.type() == TOK_COMMA )
		{
			list.emplace_back( new chain_expr( result.begin(), result.end() ) );
			result.clear();
			next_token();
		}

		e.reset();
		bool nosymbol = true;
		while ( _token.type() == TOK_SYMBOL )
		{
			if ( _token.value().back() == U'.' && _token.size() > 1 )
				_token.split( _token.size() - 1 );

			if ( _token.value() == U"." )
			{
				next_token();
				if ( _token.type() != TOK_IDENTIFIER )
					add_error( "expected feature name after '.' operator" );
				else
				{
					nosymbol = true;
					result.emplace_back( new feature_expr( std::move( _token.value() ) ) );
					next_token();
				}
			}
			else
			{
				nosymbol = false;
				result.emplace_back( new operator_expr( _token.value() ) );
				next_token();
			}
		}

		if ( !result.empty() && nosymbol && _token.type() != TOK_PAREN_START )
		{
			switch ( _token.type() )
			{
				case TOK_STRING:
				case TOK_NUMBER:
				case TOK_CHARACTER:
				case TOK_IDENTIFIER:
					add_warning( "expression does not end properly" );
					break;

				case TOK_UNKNOWN:
				case TOK_SYMBOL:
				case TOK_COMMA:
				case TOK_BLOCK_START:
				case TOK_BLOCK_END:
				case TOK_PAREN_END:
				case TOK_STATEMENT_END:
				case TOK_KEYWORD:
				case TOK_COMMENT:
				case TOK_COMMENT_BLOCK:
				case TOK_PAREN_START:
					break;
			}
			break;
		}

		e = primary_expr();
		if ( e )
		{
			auto paren = std::dynamic_pointer_cast<aggregate_expr>( e );
			if ( !result.empty() )
			{
				if ( paren && result.back()->can_be_feature() )
					result.emplace_back( new call_expr( paren->value() ) );
				else if ( nosymbol )
				{
					if ( paren->value()->is_list() )
					{
						add_error( "missing feature name or operator" );
						result.emplace_back( new error_expr( "missing feature name or operator" ) );
					}
					else
					{
						add_error( "missing feature name for call" );
						result.emplace_back( new error_expr( "missing feature name" ) );
					}

					result.emplace_back( new call_expr( paren->value() ) );
				}
				else
					result.push_back( e );
			}
			else
				result.push_back( e );
		}
	} while ( e );

	if ( list.empty() )
	{
		if ( result.empty() )
			return std::shared_ptr<expr>( new error_expr( "expected expression" ) );
		else if ( result.size() == 1 )
			return result[0];
		else
			return std::shared_ptr<expr>( new chain_expr( result.begin(), result.end() ) );
	}

	list.emplace_back( new chain_expr( result.begin(), result.end() ) );
	return std::shared_ptr<expr>( new list_expr( list.begin(), list.end() ) );
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
	}
	else if ( _token.type() == TOK_PAREN_START )
	{
		next_token();
		std::shared_ptr<expr> e = expression();
		if ( e )
		{
			if ( bool(std::dynamic_pointer_cast<aggregate_expr>( e )) )
				result = e;
			else
				result.reset( new aggregate_expr( e ) );
		}
		if ( expect( TOK_PAREN_END ) )
		{
			if ( !e )
				add_error( "empty parentheses" );
		}
		else
		{
			add_error( "missing ')' to end expression" );
			size_t here = _token.end_location().line_number();
			while ( here == _token.end_location().line_number() && _token.type() != TOK_PAREN_END )
				next_token();
			if ( _token.type() == TOK_PAREN_END )
				next_token();
		}
	}
	else if ( _token.type() == TOK_BLOCK_START )
	{
		std::cout << "starting block" << std::endl;
		next_token();
		while ( _token.type() != TOK_BLOCK_END )
			result = expression();
		expect( TOK_BLOCK_END );
	}
	else if ( _token.type() == TOK_KEYWORD )
	{
		if ( _token.value() == U"if" )
			if_expr();
		else if ( _token.value() == U"else" )
			for_expr();
	}

	return result;
}

////////////////////////////////////////

std::shared_ptr<expr> parser::paren_expr( const char *name )
{
	if ( !expect( TOK_PAREN_START ) )
		throw_runtime( "expected '(' to begin {0}", name );

	std::shared_ptr<expr> e = expression();

	if ( !expect( TOK_PAREN_END ) )
	{
		if ( e )
		{
			std::stringstream str;
			str << "expected ')' to end " << name;
			add_error( str.str() );
		}

		std::stringstream err;
		while ( true )
		{
			if ( _token.type() == TOK_PAREN_END )
			{
				next_token();
				break;
			}
			if ( _token.type() == TOK_BLOCK_START || _token.type() == TOK_BLOCK_END || _token.type() == TOK_STATEMENT_END || _token.type() == TOK_KEYWORD )
			{
				std::stringstream str;
				str << "missing ')' to end " << name;
				add_info( str.str() );
				break;
			}

			err << _token.whitespace() << _token.value();
			next_token();
		}

		if ( !err.str().empty() )
		{
			std::shared_ptr<expr> msg( new error_expr( err.str() ) );
			if ( e )
				e.reset( new chain_expr( e, msg ) );
			else
			{
				std::stringstream str;
				str << "bad " << name;
				add_error( str.str() );
				e = msg;
			}
		}
	}

	if ( !e )
	{
		std::stringstream str;
		str << "missing " << name;
		e.reset( new error_expr( str.str() ) );
		add_error( str.str() );
	}

	return e;
}

////////////////////////////////////////

std::unique_ptr<func> parser::function( void )
{
	if ( _token.type() != TOK_KEYWORD || _token.value() != U"function" )
		throw_runtime( "expected `function', got '{0}'", _token.value() );
	next_token();

	if ( _token.type() != TOK_IDENTIFIER )
		throw_runtime( "expected function name, got '{0}'", _token.value() );
	std::unique_ptr<func> f( new func( _token.value() ) );
	next_token();

	if ( expect( TOK_PAREN_START ) )
	{
		id_list( [&f]( const std::u32string &a ) { f->add_arg( a ); } );
	}
	else
		throw_runtime( "expected '(' to begin function arguments" );

	if ( !expect( TOK_PAREN_END ) )
		throw_runtime( "expected ')' to end function arguments, got '{0}'", _token.value() );

	f->set_result( expression() );

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
	if ( _token.type() != TOK_KEYWORD || _token.value() != U"if" )
		throw_runtime( "expected `if'" );
	next_token();

	auto result = std::make_shared<imgproc::if_expr>();
	result->set_condition( paren_expr( "if" ) );
	result->set_result( expression() );
	if ( _token.type() == TOK_KEYWORD && _token.value() == U"else" )
		result->set_else( expression() );
	return result;
}

////////////////////////////////////////

std::shared_ptr<expr> parser::for_expr( void )
{
	if ( _token.type() != TOK_KEYWORD || _token.value() != U"for" )
		throw_runtime( "expected `for'" );
	next_token();

	if ( _token.type() != TOK_PAREN_START )
		throw_runtime( "expected `(' after 'for', got '{0}'", _token.value() );

	auto result = std::make_shared<imgproc::for_expr>();
	id_list( [&result]( const std::u32string &a ) { result->add_variable( a ); } );

	if ( _token.type() != TOK_SYMBOL || _token.value() != U":" )
		throw_runtime( "expected ':', got '{0}'", _token.value() );

	while ( _token.type() != TOK_PAREN_START )
		result->add_range( expression() );

	if ( _token.type() != TOK_PAREN_END )
		throw_runtime( "expected `)' to close 'for', got '{0}'", _token.value() );

	result->set_result( expression() );

	return result;
}

////////////////////////////////////////

}

