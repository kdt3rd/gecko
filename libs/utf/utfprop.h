
#pragma once

#include <cstdint>

// Unicode character properties.
namespace utf
{
	// Unicode property Alphabetic 
	bool is_alphabetic( char32_t cp );

	// Unicode property Dash 
	bool is_dash( char32_t cp );

	// Unicode property Hex_Digit
	bool is_hex_digit( char32_t cp );

	// Unicode property Hyphen
	bool is_hyphen( char32_t cp );

	// Unicode property ID Continue
	bool is_identifier_continue( char32_t cp );

	// Unicode property ID Start
	bool is_identifier_start( char32_t cp );

	// Unicode property Lowercase
	bool is_lowercase( char32_t cp );

	// Unicode property Math
	bool is_math( char32_t cp );

	// Unicode property Other Alphabetic
	bool is_other_alphabetic( char32_t cp );

	// Unicode property Other Lowercase
	bool is_other_lowercase( char32_t cp );

	// Unicode property Other_Math
	bool is_other_math( char32_t cp );

	// Unicode property Other Uppercase
	bool is_other_uppercase( char32_t cp );

	// Unicode property Pattern Syntax
	bool is_pattern_syntax( char32_t cp );

	// Unicode property Pattern WS
	bool is_pattern_whitespace( char32_t cp );

	// Unicode property Question Mark
	bool is_question_mark( char32_t cp );

	// Unicode property Terminal Punctuation
	bool is_terminal_punctuation( char32_t cp );

	// Unicode property Uppercase
	bool is_uppercase( char32_t cp );

	// Unicode property White_Space
	bool is_whitespace( char32_t cp );

	// Unicode property NChar
	bool is_non_character( char32_t code_point );

	// Unicode property Nu
	bool is_numeric( char32_t cp );

	// Unicode property De
	bool is_decimal( char32_t cp );

	// Unicode property Di
	bool is_digit( char32_t cp );

	// Hexadecimal digit values (returns 0xFF if it's not a hex digit)
	uint8_t hex_value( char32_t cp );

	// The value of a number character
	double number_value( char32_t cp );

	// The integer value of a number character
	int64_t integer_value( char32_t cp );
}

