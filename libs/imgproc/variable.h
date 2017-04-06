//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "data_type.h"

namespace imgproc
{

////////////////////////////////////////

/// @brief Variable name and type.
class variable
{
public:
	variable( const std::u32string &n, const data_type &t )
		: _name( n ), _type( t )
	{
	}

	const std::u32string &name( void ) const
	{
		return _name;
	}

	const data_type &type( void ) const
	{
		return _type;
	}

private:
	std::u32string _name;
	data_type _type;
};

////////////////////////////////////////

}

