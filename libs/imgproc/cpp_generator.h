
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

	void add_functions( const std::vector<std::shared_ptr<function>> &funcs );

	var_type compile( const std::shared_ptr<function> &f, const std::vector<var_type> &args );

	std::shared_ptr<function> get_function( const std::u32string &name );

private:

	var_type result_type( const std::shared_ptr<expr> &e, const std::shared_ptr<scope> &sc );

	std::ostream &_cpp;
	std::shared_ptr<scope> _globals;

	std::map<std::pair<std::u32string,std::vector<var_type>>,var_type> _compiled;
};

////////////////////////////////////////

}

