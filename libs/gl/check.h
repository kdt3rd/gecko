//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdlib>

#define checkgl()  do { gl::error_check( __LINE__ ); } while ( false )

namespace gl
{
	void error_check( size_t line );
}
