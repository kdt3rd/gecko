
#pragma once

#include <string>

namespace utf
{

////////////////////////////////////////

// The canonical combining class.
uint8_t canonical_combining_class( char32_t cp );

// Canonical decomposition, appending the result to the string.
void canonical_decompose( char32_t cp, std::u32string &str );

// Compatibility decomposition, appending the result to the string.
void compatibility_decompose( char32_t cp, std::u32string &str );

// Compose a pair of characters (or 0 if none)
uint16_t compose_base( char32_t base );
uint8_t compose_combine( char32_t comb );
char32_t compose_char( uint32_t b, uint32_t c );

////////////////////////////////////////

}

