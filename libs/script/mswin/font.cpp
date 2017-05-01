//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font.h"

#include <script/extents.h>
#include <base/contract.h>

namespace script { namespace mswin
{

////////////////////////////////////////

font::font( std::string fam, std::string style, double pixsize )
		: script::font( std::move( fam ), std::move( style ), pixsize )
{
	throw_not_yet();
}

////////////////////////////////////////

font::~font( void )
{
}

////////////////////////////////////////

double
font::kerning( char32_t c1, char32_t c2 )
{
	throw_not_yet();

	return 0.0;
}

////////////////////////////////////////

const text_extents &
font::get_glyph( char32_t char_code )
{
	throw_not_yet();
}

////////////////////////////////////////

} }

