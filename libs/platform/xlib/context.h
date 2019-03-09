// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

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

	void share( const ::base::context &o ) override;

    XVisualInfo *choose_best_config( void );
    void create( Window w, context *sharectxt );

    ////////////////////////////////////////

    render_query render_query_func( void ) override;

	void set_viewport( coord_type x, coord_type y, coord_type w, coord_type h ) override;

    void swap_buffers( void ) override;

    GLXContext os_context( void ) const { return _ctxt; }
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
