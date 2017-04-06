//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "area.h"

namespace layout
{

////////////////////////////////////////

area::area( const std::string &n )
	: _left( n + ".l" ) , _right( n + base::to_string( ".r" ) ) , _top( n + base::to_string( ".t" ) ) , _bottom( n + base::to_string( ".b" ) ), _min_width( n + ".minw" ), _min_height( n + ".minh" )
{
}

////////////////////////////////////////

area::~area( void )
{
}

////////////////////////////////////////

}

