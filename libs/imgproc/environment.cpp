
#include "environment.h"
#include "function.h"

namespace imgproc
{

////////////////////////////////////////

std::shared_ptr<expr> infer( const function &f, const std::vector<var_type> &arg_types )
{
	precondition( arg_types.size() == f.args().size(), "mismatch for argument types" );

	std::shared_ptr<expr> result = f.result()->clone();

	environment env;
	for ( size_t i = 0; i < arg_types.size(); ++i )
		env[f.args()[i]] = type_operator( arg_types[i] );

	env.visit( result );
	return result;
}

////////////////////////////////////////

}

