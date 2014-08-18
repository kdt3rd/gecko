
#include "expr_parser.h"
#include "operators.h"

namespace imgproc
{

////////////////////////////////////////

expr_parser::expr_parser( iterator &it )
	: _it( it )
{
	_token = next_token();
}

////////////////////////////////////////

std::shared_ptr<expr> expr_parser::expression( int64_t rbp )
{
	auto t = _token;
	_token = next_token();

	auto left = t.second->right( *this, t.first );

	while ( rbp < _token.second->lbp() )
	{
		t = _token;
		_token = next_token();
		left = t.second->left( *this, t.first, left );
	}

	return left;
}

////////////////////////////////////////

void expr_parser::match( std::u32string &op )
{
	if ( op != _token.first )
		throw_runtime( "expected '{0}', got '{1}'", op, _token.first );
	_token = next_token();
}

////////////////////////////////////////

std::pair<std::u32string,std::shared_ptr<base_operator>> expr_parser::next_token( void )
{
	if ( !_it.next() )
		return std::make_pair( U"EOF", std::make_shared<end_operator>() );

	auto oplookup = operators.find( _it.value() );

	std::shared_ptr<base_operator> op;
	if ( oplookup == operators.end() )
	{
		if ( _it.type() == TOK_NUMBER )
			op = std::make_shared<primary_operator>();
		else
			throw_runtime( "unknown operator '{0}'", _it.value() );
	}
	else
		op = oplookup->second;

	auto result = std::make_pair( _it.value(), op );

	return result;
}

////////////////////////////////////////

}

