//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

namespace utf
{
	/// @brief Unicode graphic type
	bool is_graphic( char32_t code_point );

	/// @brief Unicode format type
	bool is_format( char32_t code_point );

	/// @brief Unicode control type
	bool is_control( char32_t code_point );

	/// @brief Unicode private-use type
	bool is_private( char32_t code_point );

	/// @brief Unicode surrogate type
	bool is_surrogate( char32_t code_point );

	/// @brief Unicode reserved type
	bool is_reserved( char32_t code_point );

	/// @brief Unicode category C
	bool is_other( char32_t code_point );

	/// @brief Unicode category Cc
	bool is_other_control( char32_t code_point );

	/// @brief Unicode category Cf
	bool is_other_format( char32_t code_point );

	/// @brief Unicode category Cn
	bool is_other_not_assigned( char32_t code_point );

	/// @brief Unicode category Co
	bool is_other_private( char32_t code_point );

	/// @brief Unicode category Cs
	bool is_other_surrogate( char32_t code_point );

	/// @brief Unicode category L
	bool is_letter( char32_t code_point );

	/// @brief Unicode category Ll
	bool is_letter_lowercase( char32_t code_point );

	/// @brief Unicode category Lm
	bool is_letter_modifier( char32_t code_point );

	/// @brief Unicode category Lo
	bool is_letter_other( char32_t code_point );

	/// @brief Unicode category Lt
	bool is_letter_titlecase( char32_t code_point );

	/// @brief Unicode category Lu
	bool is_letter_uppercase( char32_t code_point );

	/// @brief Unicode category M
	bool is_mark( char32_t code_point );

	/// @brief Unicode category Mc
	bool is_mark_combining( char32_t code_point );

	/// @brief Unicode category Me
	bool is_mark_enclosing( char32_t code_point );

	/// @brief Unicode category Mn
	bool is_mark_nonspacing( char32_t code_point );

	/// @brief Unicode category N
	bool is_number( char32_t code_point );

	/// @brief Unicode category Nd
	bool is_number_decimal( char32_t code_point );

	/// @brief Unicode category Nl
	bool is_number_letter( char32_t code_point );

	/// @brief Unicode category No
	bool is_number_other( char32_t code_point );

	/// @brief Unicode category P
	bool is_punctuation( char32_t code_point );

	/// @brief Unicode category Pc
	bool is_punctuation_connector( char32_t code_point );

	/// @brief Unicode category Pd
	bool is_punctuation_dash( char32_t code_point );

	/// @brief Unicode category Pe
	bool is_punctuation_close( char32_t code_point );

	/// @brief Unicode category Pf
	bool is_punctuation_final( char32_t code_point );

	/// @brief Unicode category Pi
	bool is_punctuation_initial( char32_t code_point );

	/// @brief Unicode category Po
	bool is_punctuation_other( char32_t code_point );

	/// @brief Unicode category Ps
	bool is_punctuation_open( char32_t code_point );

	/// @brief Unicode category S
	bool is_symbol( char32_t code_point );

	/// @brief Unicode category Sc
	bool is_symbol_currency( char32_t code_point );

	/// @brief Unicode category Sk
	bool is_symbol_modifier( char32_t code_point );

	/// @brief Unicode category Sm
	bool is_symbol_math( char32_t code_point );

	/// @brief Unicode category So
	bool is_symbol_other( char32_t code_point );

	/// @brief Unicode category Z
	bool is_separator( char32_t code_point );

	/// @brief Unicode category Zl
	bool is_separator_line( char32_t code_point );

	/// @brief Unicode category Zp
	bool is_separator_paragraph( char32_t code_point );

	/// @brief Unicode category Zs
	bool is_separator_space( char32_t code_point );

	/// @brief Unicode category is not known
	bool is_unknown( char32_t code_point );
}

