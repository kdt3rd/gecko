
#pragma once

// Classification of unicode code points by basic types and general category.
namespace utf
{
	// Unicode graphic type
	bool is_graphic( char32_t code_point );

	// Unicode format type
	bool is_format( char32_t code_point );

	// Unicode control type
	bool is_control( char32_t code_point );

	// Unicode private-use type
	bool is_private( char32_t code_point );

	// Unicode surrogate type
	bool is_surrogate( char32_t code_point );

	// Unicode reserved type
	bool is_reserved( char32_t code_point );

	// Unicode category C
	bool is_other( char32_t code_point );

	// Unicode category Cc
	bool is_other_control( char32_t code_point );

	// Unicode category Cf
	bool is_other_format( char32_t code_point );

	// Unicode category Cn
	bool is_other_not_assigned( char32_t code_point );

	// Unicode category Co
	bool is_other_private( char32_t code_point );

	// Unicode category Cs
	bool is_other_surrogate( char32_t code_point );

	// Unicode category L
	bool is_letter( char32_t code_point );

	// Unicode category Ll
	bool is_letter_lowercase( char32_t code_point );

	// Unicode category Lm
	bool is_letter_modifier( char32_t code_point );

	// Unicode category Lo
	bool is_letter_other( char32_t code_point );

	// Unicode category Lt
	bool is_letter_titlecase( char32_t code_point );

	// Unicode category Lu
	bool is_letter_uppercase( char32_t code_point );

	// Unicode category M
	bool is_mark( char32_t code_point );

	// Unicode category Mc
	bool is_mark_combining( char32_t code_point );

	// Unicode category Me
	bool is_mark_enclosing( char32_t code_point );

	// Unicode category Mn
	bool is_mark_nonspacing( char32_t code_point );

	// Unicode category N
	bool is_number( char32_t code_point );

	// Unicode category Nd
	bool is_number_decimal( char32_t code_point );

	// Unicode category Nl
	bool is_number_letter( char32_t code_point );

	// Unicode category No
	bool is_number_other( char32_t code_point );

	// Unicode category P
	bool is_punctuation( char32_t code_point );

	// Unicode category Pc
	bool is_punctuation_connector( char32_t code_point );

	// Unicode category Pd
	bool is_punctuation_dash( char32_t code_point );

	// Unicode category Pe
	bool is_punctuation_close( char32_t code_point );

	// Unicode category Pf
	bool is_punctuation_final( char32_t code_point );

	// Unicode category Pi
	bool is_punctuation_initial( char32_t code_point );

	// Unicode category Po
	bool is_punctuation_other( char32_t code_point );

	// Unicode category Ps
	bool is_punctuation_open( char32_t code_point );

	// Unicode category S
	bool is_symbol( char32_t code_point );

	// Unicode category Sc
	bool is_symbol_currency( char32_t code_point );

	// Unicode category Sk
	bool is_symbol_modifier( char32_t code_point );

	// Unicode category Sm
	bool is_symbol_math( char32_t code_point );

	// Unicode category So
	bool is_symbol_other( char32_t code_point );

	// Unicode category Z
	bool is_separator( char32_t code_point );

	// Unicode category Zl
	bool is_separator_line( char32_t code_point );

	// Unicode category Zp
	bool is_separator_paragraph( char32_t code_point );

	// Unicode category Zs
	bool is_separator_space( char32_t code_point );

	// Unicode category is not known
	bool is_unknown( char32_t code_point );
}

