//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/context.h>
#include <gl/opengl.h>
#include <EGL/egl.h>

////////////////////////////////////////

namespace platform
{

namespace wayland
{

///
/// @brief Class context provides...
///
class context : public ::platform::context
{
public:
    using render_query = ::platform::context::render_query;

    explicit context( EGLDisplay disp );
	~context( void ) override;

    void create( EGLNativeWindowType nwin );

    ////////////////////////////////////////

	void share( const ::base::context &o ) override;

    render_query render_query_func( void ) override;

	void set_viewport( coord_type x, coord_type y, coord_type w, coord_type h ) override;

    void swap_buffers( void ) override;

protected:
	void acquire( void ) override;
	void release( void ) override;

	void reset_clip( const rect &r ) override;
    
private:
    EGLDisplay _disp;
    EGLContext _ctxt;
	EGLSurface _surface = nullptr;
};

} // namespace wayland

} // namespace platform



