// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

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
