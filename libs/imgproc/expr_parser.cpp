
#include "expr_parser.h"
#include "operators.h"

namespace imgproc
{

////////////////////////////////////////

expr_parser::expr_parser( iterator &it, const std::function<std::shared_ptr<expr>(void)> &primary )
	: _it( it ), _primary( primary )
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
	if ( _gottoken )
		_it.next();

	std::pair<std::u32string,std::shared_ptr<base_operator>> result;
	if ( !_it )
	{
		result = std::make_pair( std::u32string(), std::make_shared<end_operator>() );
		return result;
	}

	_gottoken = true;
	if ( _it.type() == TOK_OPERATOR )
	{
		std::u32string opname = _it.value();

		auto oplookup = operators.find( opname );
		while ( !opname.empty() && oplookup == operators.end() )
		{
			opname.pop_back();
			oplookup = operators.find( opname );
		}

		if ( oplookup == operators.end() )
			throw_runtime( "unknown operator '{0}'", _it.value() );

		_it.split( opname );

		std::shared_ptr<base_operator> op( oplookup->second );
		result = std::make_pair( opname, op );
	}
	else
	{
		auto op = std::make_shared<primary_operator>( _primary() );
		if ( op )
		{
			_gottoken = false;
			result = std::make_pair( _it.value(), op );
		}
		else
			result = std::make_pair( _it.value(), std::make_shared<end_operator>() );
	}
	return result;
}

////////////////////////////////////////

}

