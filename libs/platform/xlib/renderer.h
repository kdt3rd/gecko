//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once


#include <platform/renderer.h>

////////////////////////////////////////

namespace platform
{

namespace xlib
{

///
/// @brief Class renderer provides...
///
class renderer : public ::platform::renderer
{
public:
    using render_query = ::platform::renderer::render_query;

    renderer( void );
	~renderer( void ) override;

    std::shared_ptr<context> create_context( const std::shared_ptr<::platform::window> &w ) override;

	void activate( const std::shared_ptr<context> &c ) override;
	void deactivate( const std::shared_ptr<context> &c ) override;

    render_query render_query_func( void ) override;

private:

};

} // namespace xlib

} // namespace platform



