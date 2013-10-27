
#pragma once

#include <memory>
#include "paint.h"
#include "path.h"
#include "font.h"

namespace draw
{

////////////////////////////////////////

/// @brief Canvas to draw on
class canvas
{
public:

	/// @brief Constructor
	canvas( void );

	/// @brief Destructor
	virtual ~canvas( void );

	/// @brief Fill the entire canvas
	/// @param c Color to fill with
	void fill( const color &c );

	/// @brief Fill the entire canvas
	/// @param p Paint to fill with
	virtual void fill( const paint &p ) = 0;

	/// @brief Draw a path
	/// Draw a path on the canvas
	/// @param p Path to draw
	/// @param c Paint to apply to the path
	virtual void draw_path( const path &p, const paint &c ) = 0;

	/// @brief Draw text
	/// Draw text on the canvas
	/// @param font The font to draw with
	/// @param p The start of the baseline
	/// @param utf8 The text to draw
	/// @param c The paint to color with
	virtual void draw_text( const std::shared_ptr<font> &font, const point &p, const std::string &utf8, const paint &c ) = 0;

	/// @brief Present the canvas
	/// Finalize the drawing of the canvas and display it.
	virtual void present( void );
};

////////////////////////////////////////

}
