// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "scancode.h"
#include "waitable.h"

#include <functional>

namespace platform
{
////////////////////////////////////////

/// @brief Keyboard device.
///
/// A keyboard device that can send key press/release events.
class keyboard : public waitable
{
public:
    /// @brief Constructor.
    keyboard( system *s );

    /// @brief Destructor.
    virtual ~keyboard( void );

private:
};

////////////////////////////////////////

} // namespace platform
