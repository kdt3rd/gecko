
#pragma once

#include "expr.h"
#include "token.h"

namespace imgproc
{

class base_operator;

////////////////////////////////////////

class expr_parser
{
public:
	expr_parser( iterator &tok );

	std::shared_ptr<expr> expression( int64_t rbp = 0 );

	void match( std::u32string &op );

private:
	std::pair<std::u32string,std::shared_ptr<base_operator>> next_token( void );
	std::pair<std::u32string,std::shared_ptr<base_operator>> _token;
	iterator &_it;
};

////////////////////////////////////////

}

