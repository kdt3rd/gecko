// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#include <iterator>
#include "function.h"

namespace imgproc
{

////////////////////////////////////////

std::shared_ptr<function> function::clone( void ) const
{
	auto result = std::make_shared<function>( _name );
	result->set_result( _result->clone() );
	for ( size_t i = 0; i < _args.size(); ++i )
		result->add_arg( _args[i], _mods[i] );
	return result;
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const function &f )
{
	if ( f.name().empty() )
		out << "lambda ( ";
	else
		out << "function " << f.name() << "( ";

	if ( !f.args().empty() )
	{
		std::copy( f.args().begin(), f.args().end()-1, std::ostream_iterator<std::u32string>( out, ", " ) );
		out << f.args().back();
	}
	out << " )\n{\n    ";
	out << f.result();
	out << ";\n}\n";

	return out;
}

////////////////////////////////////////

}

