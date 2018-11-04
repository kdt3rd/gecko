//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <chrono>
#include <cstdint>
#include <string>

////////////////////////////////////////

namespace base
{

std::string as_ISO8601( std::chrono::system_clock::time_point tp );
int64_t local_offset_to_UTC( void );

} // namespace base



