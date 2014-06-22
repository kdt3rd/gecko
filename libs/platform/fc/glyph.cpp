
#include "glyph.h"
#include <stdexcept>

namespace platform { namespace fc
{

////////////////////////////////////////

glyph::glyph( FT_Face face, wchar_t code )
		: ::draw::glyph( code ),
		  _face( face ),
		  _index( FT_Get_Char_Index( face, code ) )
{
	if ( _index == 0 )
		throw std::runtime_error( "Undefined character code" );
}


////////////////////////////////////////


glyph::~glyph( void )
{
}


////////////////////////////////////////


double
glyph::load_kerning( wchar_t prev_char ) const
{
	FT_Vector kerning;
	FT_UInt prev_index = FT_Get_Char_Index( _face, prev_char );

	FT_Get_Kerning( _face, prev_index, _index, FT_KERNING_UNFITTED, &kerning );

	double rval = 0.0;
	if ( kerning.x )
	{
		rval = kerning.x / ( 64.0 * 64.0 );
	}

	_kerning[prev_char] = rval;
	return rval;
}

////////////////////////////////////////

} }
