// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <platform/keyboard.h>

namespace platform
{
namespace dummy
{
////////////////////////////////////////

/// @brief Dummy keyboard class
class keyboard : public platform::keyboard
{
public:
    keyboard( void );
    ~keyboard( void );
};

////////////////////////////////////////

} // namespace dummy
} // namespace platform
