
#pragma once

#include <map>

#include "extents.h"

////////////////////////////////////////

namespace script
{

///
/// @brief Storage to represent a glyph for font rendering
/// @author Kimball Thurston
///
/// vertical layout isn't really designed into the API currently
/// as the layout is 
///
/// Glyph metrics:
/// --------------
///  for horizontal layout...
///
///                       xmin                     xmax
///                        |                         |
///                        |<-------- width -------->|
///                        |                         |    
///              |         +-------------------------+----------------- ymax
///              |         |    ggggggggg   ggggg    |     ^        ^
///              |         |   g:::::::::ggg::::g    |     |        | 
///              |         |  g:::::::::::::::::g    |     |        | 
///              |         | g::::::ggggg::::::gg    |     |        | 
///              |         | g:::::g     g:::::g     |     |        | 
///   x_bearing -|-------->| g:::::g     g:::::g     |  y_bearing   | 
///              |         | g:::::g     g:::::g     |     |        | 
///              |         | g::::::g    g:::::g     |     |        | 
///              |         | g:::::::ggggg:::::g     |     |        |  
///              |         |  g::::::::::::::::g     |     |      height
///              |         |   gg::::::::::::::g     |     |        | 
///  baseline ---*---------|---- gggggggg::::::g-----*--------      |
///            / |         |             g:::::g     |              | 
///     origin   |         | gggggg      g:::::g     |              | 
///              |         | g:::::gg   gg:::::g     |              | 
///              |         |  g::::::ggg:::::::g     |              | 
///              |         |   gg:::::::::::::g      |              | 
///              |         |     ggg::::::ggg        |              | 
///              |         |         gggggg          |              v
///              |         +-------------------------+----------------- ymin
///              |                                   |
///              |------------- advance_x ---------->|
///
/// In freetype, the offset_XXX parameters are called Bearing, which one
/// you select is based on horizontal or vertical layout
///
class glyph
{
public:
	glyph( wchar_t code );
	virtual ~glyph( void );

	wchar_t code( void ) const { return _code; }
	text_extents &extents( void ) { return _extents; }
	const text_extents &extents( void ) const { return _extents; }

	double kerning( wchar_t prev_char ) const;

protected:
	virtual double load_kerning( wchar_t prev_char ) const = 0;

	wchar_t _code;
	text_extents _extents;

	mutable std::map<wchar_t, double> _kerning;
};

}

