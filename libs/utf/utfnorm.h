
#pragma once

#include <string>

namespace utf
{

////////////////////////////////////////

/// @brief The canonical combining class.
///
/// The combining class indicates the priority with which a combining character is attached to its base character.
uint8_t canonical_combining_class( char32_t cp );

/// @brief Canonical decomposition, appending the result to the string.
void canonical_decompose( char32_t cp, std::u32string &str );

/// @brief Compatibility decomposition, appending the result to the string.
void compatibility_decompose( char32_t cp, std::u32string &str );

/// @cond UTF
uint16_t compose_base( char32_t base );
uint8_t compose_combine( char32_t comb );
char32_t compose_char( uint32_t b, uint32_t c );
/// @endcond

////////////////////////////////////////

}

