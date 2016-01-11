
#pragma once

#include "unifier.h"
#include "expr.h"
#include <utf/utf.h>
#include <limits>

namespace imgproc
{

class function;

////////////////////////////////////////

class environment
{
public:
	environment( std::map<std::u32string,std::shared_ptr<function>> &f );

	type operator()( const integer_expr &e );
	type operator()( const floating_expr &e );
	type operator()( const identifier_expr &e );
	type operator()( const prefix_expr &e );
	type operator()( const postfix_expr &e );
	type operator()( const infix_expr &e );
	type operator()( const circumfix_expr &e );
	type operator()( const postcircumfix_expr &e );
	type operator()( const call_expr &e );
	type operator()( const if_expr &e );
	type operator()( const range_expr &e );
	type operator()( const for_expr &e );
	type operator()( const assign_expr &e );
	type operator()( const lambda_expr &e );

	// Compile-time check for any missing operator() implementation
	template<typename T>
	type operator()( T /*a*/ )
	{
		static_assert( base::always_false<T>::value, "missing operator() for variant types" );
		return type();
	}

	std::shared_ptr<expr> infer( const function &f, std::vector<type> &arg_types );

private:
	typedef std::map<std::u32string,type> scope;

	std::shared_ptr<expr> unify( const function &f, std::vector<type> &arg_types );
	std::shared_ptr<expr> unify( const function &f, type_callable &call );

	type find_scope( const std::u32string &name );
	void add_scope( const std::u32string &name, const type &ty );

	type visit( const std::shared_ptr<expr> &e );

	type new_type( void );

	type join( const type_primary &t1, const type_primary &t2 );

	size_t _type_id = 0;
	unifier _unify;
	std::vector<scope> _env;
	std::vector<std::shared_ptr<expr>> _current;
	std::map<std::u32string,std::shared_ptr<function>> &_funcs;
	std::map<const void *,std::shared_ptr<expr>> _func_exprs;
	std::map<size_t,std::shared_ptr<expr>> _var_exprs;
};

////////////////////////////////////////

}

