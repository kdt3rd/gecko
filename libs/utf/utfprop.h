
#pragma once

#include <cstdint>

namespace utf
{
	/// @brief Unicode property Alphabetic 
	bool is_alphabetic( char32_t cp );

	/// @brief Unicode property Dash 
	bool is_dash( char32_t cp );

	/// @brief Unicode property Hex_Digit
	bool is_hex_digit( char32_t cp );

	/// @brief Unicode property Hyphen
	bool is_hyphen( char32_t cp );

	/// @brief Unicode property ID Continue
	bool is_identifier_continue( char32_t cp );

	/// @brief Unicode property ID Start
	bool is_identifier_start( char32_t cp );

	/// @brief Unicode property Lowercase
	bool is_lowercase( char32_t cp );

	/// @brief Unicode property Math
	bool is_math( char32_t cp );

	/// @brief Unicode property Other Alphabetic
	bool is_other_alphabetic( char32_t cp );

	/// @brief Unicode property Other Lowercase
	bool is_other_lowercase( char32_t cp );

	/// @brief Unicode property Other_Math
	bool is_other_math( char32_t cp );

	/// @brief Unicode property Other Uppercase
	bool is_other_uppercase( char32_t cp );

	/// @brief Unicode property Pattern Syntax
	bool is_pattern_syntax( char32_t cp );

	/// @brief Unicode property Pattern WS
	bool is_pattern_whitespace( char32_t cp );

	/// @brief Unicode property Question Mark
	bool is_question_mark( char32_t cp );

	/// @brief Unicode property Terminal Punctuation
	bool is_terminal_punctuation( char32_t cp );

	/// @brief Unicode property Uppercase
	bool is_uppercase( char32_t cp );

	/// @brief Unicode property White_Space
	bool is_whitespace( char32_t cp );

	/// @brief Unicode property NChar
	bool is_non_character( char32_t code_point );

	/// @brief Unicode property Nu
	bool is_numeric( char32_t cp );

	/// @brief Unicode property De
	bool is_decimal( char32_t cp );

	/// @brief Unicode property Di
	bool is_digit( char32_t cp );

	/// @brief Hexadecimal digit values (returns 0xFF if it's not a hex digit)
	uint8_t hex_value( char32_t cp );

	/// @brief The value of a number character
	double number_value( char32_t cp );

	/// @brief The integer value of a number character
	uint64_t integer_value( char32_t cp );
}

