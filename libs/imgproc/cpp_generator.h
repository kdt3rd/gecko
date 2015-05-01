
#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "data_type.h"
#include "expr.h"
#include "scope.h"
#include <base/variant.h>

namespace imgproc
{

////////////////////////////////////////

class cpp_generator
{
public:
	cpp_generator( std::ostream &cpp );

	std::string operator()( const integer_expr &e );
	std::string operator()( const floating_expr &e );
	std::string operator()( const identifier_expr &e );
	std::string operator()( const prefix_expr &e );
	std::string operator()( const postfix_expr &e );
	std::string operator()( const infix_expr &e );
	std::string operator()( const circumfix_expr &e );
	std::string operator()( const postcircumfix_expr &e );
	std::string operator()( const call_expr &e );
	std::string operator()( const if_expr &e );
	std::string operator()( const range_expr &e );
	std::string operator()( const for_expr &e );
	std::string operator()( const assign_expr &e );
	std::string operator()( const lambda_expr &e );

	// Compile-time check for any missing operator() implementation
	template<typename T>
	std::string operator()( T a )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
		return std::string();
	}

//	void add_functions( const std::map<std::u32string,std::shared_ptr<function>> &f );

//	data_type compile( const std::u32string &func );
	type compile( const function &f, const std::vector<type> &arg_types );

//	std::shared_ptr<function> get_function( const std::u32string &name );

private:
	std::string visit( const std::shared_ptr<expr> &e );

	std::ostream &_cpp;
	std::shared_ptr<scope> _globals;
	type _type;
	uint32_t _tmp = 1;
	std::map<std::pair<std::u32string,std::vector<data_type>>,data_type> _compiled;
};

////////////////////////////////////////

}

