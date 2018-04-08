//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>
#include <cstdint>
#include <cstddef>

////////////////////////////////////////

namespace platform
{

using context = uintptr_t;
class window;

///
/// @brief Class renderer provides an abstraction around the selected rendering engine.
///
/// This currently largely exists as a placeholder around OpenGL, such
/// that there is a convenient place for platforms to store a shared
/// context or whatever they need to do.
///
/// In the future, this should morph to support more native rendering,
/// to enable UI over the network, or whatever is necessary.
///
class renderer
{
public:
	// least common denominator so far (X protocol for drawing
	// rectangles, etc. only allows short)
	using coord_type = int16_t;
	using size_type = uint16_t;

	using render_func_ptr = void (*)( void );
	using render_query = render_func_ptr (*)( const char * );

	renderer( void );
	virtual ~renderer( void );

	virtual std::shared_ptr<context> create_context( const std::shared_ptr<window> &w ) = 0;

	virtual void activate( const std::shared_ptr<context> &c ) = 0;
	virtual void deactivate( const std::shared_ptr<context> &c ) = 0;

	virtual render_query render_query_func( void ) = 0;

private:

};

} // namespace platform



