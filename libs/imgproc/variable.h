
#pragma once

#include "data_type.h"

namespace imgproc
{

////////////////////////////////////////

class variable
{
public:
	variable( const std::u32string &n, const type &t )
		: _name( n ), _type( t )
	{
	}

	const std::u32string &name( void ) const
	{
		return _name;
	}

	const type &get_type( void ) const
	{
		return _type;
	}

private:
	std::u32string _name;
	imgproc::type _type;
};

////////////////////////////////////////

}

