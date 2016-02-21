
#pragma once

namespace base
{

/// @brief ANSI escape sequences
namespace ansi
{
	/// @brief Reset back to default color/style
	constexpr const char *reset = "\x1B[0m";

	/// @brief Switch to black text
	constexpr const char *black = "\x1B[30m";

	/// @brief Switch to red text
	constexpr const char *red = "\x1B[31m";

	/// @brief Switch to green text
	constexpr const char *green = "\x1B[32m";

	/// @brief Switch to yellow text
	constexpr const char *yellow = "\x1B[33m";

	/// @brief Switch to blue text
	constexpr const char *blue = "\x1B[34m";

	/// @brief Switch to magenta text
	constexpr const char *magenta = "\x1B[35m";

	/// @brief Switch to cyan text
	constexpr const char *cyan = "\x1B[36m";

	/// @brief Switch to white text
	constexpr const char *white = "\x1B[37m";

	/// @brief Switch to underline text
	constexpr const char *underline = "\x1B[4m";

	/// @brief Switch to invert text
	constexpr const char *invert = "\x1B[7m";
}

}

