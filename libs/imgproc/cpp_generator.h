
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

	void compile( const std::shared_ptr<func> &f, const std::shared_ptr<scope> &scope, std::stringstream &code );

	std::ostream &_cpp;
	std::map<std::u32string,std::shared_ptr<func>> _funcs;
};

////////////////////////////////////////

}

