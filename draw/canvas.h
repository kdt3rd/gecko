
#pragma once

#include <memory>
#include "paint.h"
#include "path.h"

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
	///
	/// @param p Path to draw
	/// @param c Paint to apply to path
	virtual void draw_path( const std::shared_ptr<path> &p, const paint &c ) = 0;

};

////////////////////////////////////////

}

