
#pragma once

#include <draw/glyph.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace fc
{

class glyph : public draw::glyph
{
public:
	glyph( FT_Face face, wchar_t code );
	virtual ~glyph( void );

	inline FT_UInt index( void ) const { return _index; }

protected:
	virtual double load_kerning( wchar_t prev_char ) const override;

private:
	FT_Face _face;
	FT_UInt _index = 0;
};

}


