//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/context.h>
#include <X11/Xlib.h>
#include <gl/opengl.h>
#include <GL/glx.h>

////////////////////////////////////////

namespace platform
{

namespace xlib
{

///
/// @brief Class context provides...
///
class context : public ::platform::context
{
public:
    using render_query = ::platform::context::render_query;

    explicit context( const std::shared_ptr<Display> &dpy );
	~context( void ) override;

    XVisualInfo *choose_best_config( void );
    void create( Window w );

    ////////////////////////////////////////

    render_query render_query_func( void ) override;

	void share( ::platform::context &o ) override;

	void set_viewport( coord_type x, coord_type y, coord_type w, coord_type h ) override;

    void swap_buffers( void ) override;

protected:
	void acquire( void ) override;
	void release( void ) override;

	void reset_clip( const rect &r ) override;
    
private:
    std::shared_ptr<Display> _display;
    GLXFBConfig _bestFBC = None;
    GLXWindow _win = None;
    GLXContext _ctxt = None;
    coord_type _last_vp[4];
};

} // namespace xlib

} // namespace platform



