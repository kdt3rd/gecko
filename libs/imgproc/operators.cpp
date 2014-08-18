
#include "operators.h"
#include "expr_parser.h"

namespace imgproc
{

////////////////////////////////////////

std::map<std::u32string,std::shared_ptr<base_operator>> operators =
{
	{ U"-", std::make_shared<preinfix_operator>( 100, 10 ) },
	{ U"+", std::make_shared<preinfix_operator>( 100, 10 ) },
	{ U"!", std::make_shared<postfix_operator>( 90 ) },
	{ U"*", std::make_shared<infix_operator>( 20 ) },
	{ U"/", std::make_shared<infix_operator>( 20 ) },
	{ U"**", std::make_shared<infix_operator>( 20, false ) },
	{ U"(", std::make_shared<circumfix_operator>( U")" ) },
	{ U")", std::make_shared<base_operator>() },
	{ U"|", std::make_shared<circumfix_operator>( U"|" ) },
};

////////////////////////////////////////

base_operator::~base_operator( void )
{
}

////////////////////////////////////////

int64_t base_operator::lbp( void )
{
	return 0;
}

////////////////////////////////////////

std::shared_ptr<expr> base_operator::right( expr_parser &parser, const std::u32string &op )
{
	throw_runtime( "expected operand, got '{0}'", op );
}

////////////////////////////////////////

std::shared_ptr<expr> base_operator::left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left )
{
	throw_runtime( "expected operand, got '{0}'", op );
}

////////////////////////////////////////

std::shared_ptr<expr> primary_operator::right( expr_parser &parser, const std::u32string &op )
{
	return std::make_shared<number_expr>( op );
}

////////////////////////////////////////

int64_t end_operator::lbp( void )
{
	return 0;
}

////////////////////////////////////////

infix_operator::infix_operator( int64_t bp, bool left_assoc )
	: _bp( bp ), _left_assoc( left_assoc )
{
}

////////////////////////////////////////

int64_t infix_operator::lbp( void )
{
	return _bp;
}

////////////////////////////////////////

std::shared_ptr<expr> infix_operator::left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left )
{
	auto right = parser.expression( _bp - ( _left_assoc ? 0 : 1 ) );
	return std::make_shared<infix_expr>( op, left, right );
}

////////////////////////////////////////

prefix_operator::prefix_operator( int64_t bp )
	: _bp( bp )
{
}

////////////////////////////////////////

int64_t prefix_operator::lbp( void )
{
	return _bp;
}

////////////////////////////////////////

std::shared_ptr<expr> prefix_operator::right( expr_parser &parser, const std::u32string &op )
{
	auto operand = parser.expression( _bp );
	return std::make_shared<prefix_expr>( op, operand );
}

////////////////////////////////////////

postfix_operator::postfix_operator( int64_t bp )
	: _bp( bp )
{
}

////////////////////////////////////////

int64_t postfix_operator::lbp( void )
{
	return _bp;
}

////////////////////////////////////////

std::shared_ptr<expr> postfix_operator::left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left )
{
	return std::make_shared<postfix_expr>( op, left );
}

////////////////////////////////////////

preinfix_operator::preinfix_operator( int64_t pre_bp, int64_t in_bp )
	: _pre_bp( pre_bp ), _in_bp( in_bp )
{
}

////////////////////////////////////////

int64_t preinfix_operator::lbp( void )
{
	return _in_bp;
}

////////////////////////////////////////

std::shared_ptr<expr> preinfix_operator::left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left )
{
	auto right = parser.expression( _in_bp );
	return std::make_shared<infix_expr>( op, left, right );
}

////////////////////////////////////////

std::shared_ptr<expr> preinfix_operator::right( expr_parser &parser, const std::u32string &op )
{
	auto operand = parser.expression( _pre_bp );
	return std::make_shared<prefix_expr>( op, operand );
}

////////////////////////////////////////

circumfix_operator::circumfix_operator( const std::u32string &close )
	: _close( close )
{
}

////////////////////////////////////////

std::shared_ptr<expr> circumfix_operator::right( expr_parser &parser, const std::u32string &op )
{
	auto operand = parser.expression();
	parser.match( _close );
	return std::make_shared<closed_expr>( op, _close, operand );
}

////////////////////////////////////////

}
