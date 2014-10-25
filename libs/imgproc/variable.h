
#pragma once

#include "data_type.h"

namespace imgproc
{

////////////////////////////////////////

class variable
{
public:
	variable( const std::u32string &n, const var_type &t )
		: _name( n ), _type( t )
	{
	}

	const std::u32string &name( void ) const
	{
		return _name;
	}

	const var_type &type( void ) const
	{
		return _type;
	}

private:
	std::u32string _name;
	var_type _type;
};

////////////////////////////////////////

}

