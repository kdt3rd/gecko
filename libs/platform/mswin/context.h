// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <platform/context.h>
#include <windows.h>

////////////////////////////////////////

namespace platform
{
namespace mswin
{
///
/// @brief Class context provides...
///
class context : public ::platform::context
{
public:
    using base            = ::platform::context;
    using render_func_ptr = base::render_func_ptr;
    using render_query    = base::render_query;

    context( void );
    ~context( void ) override;

    render_query render_query_func( void ) override;

    void init( HWND hwnd );
    void share( const ::base::context &o ) override;

    void set_viewport(
        coord_type x, coord_type y, coord_type w, coord_type h ) override;
    void swap_buffers( void ) override;

protected:
    void acquire( void ) override;
    void release( void ) override;

    void reset_clip( const rect &r ) override;

private:
    HGLRC      _hrc = nullptr;
    HDC        _hdc = nullptr;
    coord_type _last_vp[4];
};

} // namespace mswin
} // namespace platform
