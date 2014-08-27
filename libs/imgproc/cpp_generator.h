
#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "data_type.h"
#include "expr.h"
#include "scope.h"

namespace imgproc
{

////////////////////////////////////////

class cpp_generator
{
public:
	cpp_generator( std::ostream &cpp );

	void add_functions( const std::vector<std::shared_ptr<func>> &funcs );

	type generate( const std::u32string &name, const std::vector<type> &args );

private:
	std::shared_ptr<func> get_function( const std::u32string &name );

	// Compile a top-level expression
	void compile( std::ostream &code, size_t indent, std::shared_ptr<scope> &sc, const std::shared_ptr<expr> &expr, const variable &result );

	type result_type( const std::shared_ptr<expr> &e, const std::shared_ptr<scope> &scope );

	std::ostream &_cpp;
	std::shared_ptr<scope> _globals = std::make_shared<scope>();;
};

////////////////////////////////////////

}

