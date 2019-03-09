// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

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



