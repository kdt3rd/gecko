//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "contract.h"
#include <windows.h>

////////////////////////////////////////

#define throw_win32_error( ... ) \
	throw_location( std::system_error( GetLastError(), std::generic_category(), base::format( __VA_ARGS__ ) ) )
