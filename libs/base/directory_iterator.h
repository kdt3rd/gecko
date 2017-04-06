//
// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "uri.h"
#include <functional>

namespace base
{

////////////////////////////////////////

class directory_iterator
{
public:
	directory_iterator( const std::function<uri(void)> &next )
		: _next( next )
	{
	}

	directory_iterator &operator++( void )
	{
		return increment();
	}

	base::uri &operator*( void )
	{
		return _uri;
	}

	base::uri *operator->( void )
	{
		return &_uri;
	}

	directory_iterator &increment( void )
	{
		_uri = _next();
		return *this;
	}

	explicit operator bool( void ) const
	{
		return bool( _uri );
	}

private:
	const std::function<uri(void)> _next;
	base::uri _uri;
};

////////////////////////////////////////

}

