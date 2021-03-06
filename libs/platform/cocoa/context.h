// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <gl/opengl.h>
#include <platform/context.h>

////////////////////////////////////////

namespace platform
{
namespace cocoa
{
///
/// @brief Class context provides...
///
class context : public ::platform::context
{
public:
    using render_query = ::platform::context::render_query;

    context();
    ~context( void ) override;

    void set_ns( void *nswin, void *nsview );

    ////////////////////////////////////////

    render_query render_query_func( void ) override;

    void share( const ::base::context &o ) override;

    void set_viewport(
        coord_type x, coord_type y, coord_type w, coord_type h ) override;

    void swap_buffers( void ) override;

protected:
    void acquire( void ) override;
    void release( void ) override;

    void reset_clip( const rect &r ) override;

private:
    void *     _window = nullptr;
    void *     _view   = nullptr;
    coord_type _last_vp[4];
};

} // namespace cocoa

} // namespace platform
