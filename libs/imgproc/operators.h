
#pragma once

#include "expr.h"
#include <map>

namespace imgproc
{

////////////////////////////////////////

class expr_parser;
class base_operator;

extern std::map<std::u32string,std::shared_ptr<base_operator>> operators;

////////////////////////////////////////

class base_operator
{
public:
	virtual ~base_operator( void );

	virtual int64_t lbp( void );
	virtual std::shared_ptr<expr> right( expr_parser &parser, const std::u32string &op );
	virtual std::shared_ptr<expr> left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left );
};

////////////////////////////////////////

class primary_operator : public base_operator
{
public:
	primary_operator( const std::shared_ptr<expr> &e );

	virtual std::shared_ptr<expr> right( expr_parser &parser, const std::u32string &op );

private:
	std::shared_ptr<expr> _expr;
};

////////////////////////////////////////

class end_operator : public base_operator
{
public:
	virtual int64_t lbp( void );
};

////////////////////////////////////////

class infix_operator : public base_operator
{
public:
	infix_operator( int64_t bp, bool leftassoc = true );

	virtual int64_t lbp( void );
	virtual std::shared_ptr<expr> left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left );

private:
	int64_t _bp;
	bool _left_assoc;
};

////////////////////////////////////////

class prefix_operator : public base_operator
{
public:
	prefix_operator( int64_t bp );

	virtual int64_t lbp( void );
	virtual std::shared_ptr<expr> right( expr_parser &parser, const std::u32string &op );

private:
	int64_t _bp;
};

////////////////////////////////////////

class postfix_operator : public base_operator
{
public:
	postfix_operator( int64_t bp );

	virtual int64_t lbp( void );
	virtual std::shared_ptr<expr> left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left );

private:
	int64_t _bp;
};

////////////////////////////////////////

class dualfix_operator : public base_operator
{
public:
	dualfix_operator( int64_t pre_bp, int64_t in_bp );

	virtual int64_t lbp( void );
	virtual std::shared_ptr<expr> right( expr_parser &parser, const std::u32string &op );
	virtual std::shared_ptr<expr> left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left );

private:
	int64_t _pre_bp, _in_bp;
};

////////////////////////////////////////

class circumfix_operator : public base_operator
{
public:
	circumfix_operator( const std::u32string &close );
	virtual std::shared_ptr<expr> right( expr_parser &parser, const std::u32string &op );

private:
	std::u32string _close;
};

////////////////////////////////////////

class postcircumfix_operator : public base_operator
{
public:
	postcircumfix_operator( const std::u32string &close );
	int64_t lbp( void ) override;
	std::shared_ptr<expr> left( expr_parser &parser, const std::u32string &op, const std::shared_ptr<expr> &left ) override;

private:
	std::u32string _close;
};

////////////////////////////////////////

}
