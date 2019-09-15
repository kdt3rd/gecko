// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

namespace base
{
/// @brief ANSI escape sequences
class ansi
{
public:
    /// @brief Reset back to default color/style
    static constexpr const char *reset = "\x1B[0m";

    /// @brief Switch to black text
    static constexpr const char *black = "\x1B[30m";

    /// @brief Switch to red text
    static constexpr const char *red = "\x1B[31m";

    /// @brief Switch to green text
    static constexpr const char *green = "\x1B[32m";

    /// @brief Switch to yellow text
    static constexpr const char *yellow = "\x1B[33m";

    /// @brief Switch to blue text
    static constexpr const char *blue = "\x1B[34m";

    /// @brief Switch to magenta text
    static constexpr const char *magenta = "\x1B[35m";

    /// @brief Switch to cyan text
    static constexpr const char *cyan = "\x1B[36m";

    /// @brief Switch to white text
    static constexpr const char *white = "\x1B[37m";

    /// @brief Switch to underline text
    static constexpr const char *underline = "\x1B[4m";

    /// @brief Switch to invert text
    static constexpr const char *invert = "\x1B[7m";
};

} // namespace base
