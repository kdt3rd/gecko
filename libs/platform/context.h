// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "types.h"

#include <base/context.h>
#include <base/utility.h>
#include <gl/api.h>
#include <stack>

////////////////////////////////////////

namespace platform
{
///
/// @brief Class context provides...
///
class context : public base::context
{
public:
    using render_func_ptr = void ( * )( void );
    using render_query    = render_func_ptr ( * )( const char * );

    class clip_region_guard
    {
    private:
        clip_region_guard( context *c, const rect &r ) : _c( c )
        {
            c->push_clip_rect( r );
        }
        friend class context;
        context *_c = nullptr;

    public:
        clip_region_guard( void )                      = delete;
        clip_region_guard( const clip_region_guard & ) = delete;
        clip_region_guard &operator=( const clip_region_guard & ) = delete;
        clip_region_guard( clip_region_guard &&o ) noexcept
            : _c( base::exchange( o._c, nullptr ) )
        {}
        clip_region_guard &operator=( clip_region_guard &&o ) noexcept
        {
            if ( this != &o )
                _c = base::exchange( o._c, nullptr );
            return *this;
        }
        ~clip_region_guard( void )
        {
            if ( _c )
                _c->pop_clip_rect();
        }
    };

    class render_guard
    {
    private:
        render_guard( context *c ) : _c( c ) { c->acquire(); }
        friend class context;
        context *_c = nullptr;

    public:
        render_guard( void )                 = delete;
        render_guard( const render_guard & ) = delete;
        render_guard &operator=( const render_guard & ) = delete;
        render_guard( render_guard &&o ) noexcept
            : _c( base::exchange( o._c, nullptr ) )
        {}
        render_guard &operator=( render_guard &&o ) noexcept
        {
            if ( this != &o )
                _c = base::exchange( o._c, nullptr );
            return *this;
        }
        ~render_guard( void )
        {
            if ( _c )
                _c->release();
        }
    };

    context( void );
    context( const context &o ) = delete;
    context &operator=( const context &o ) = delete;
    context( context &&o )                 = delete;
    context &operator=( context &&o ) = delete;
    virtual ~context( void );

    virtual render_query render_query_func( void ) = 0;

    virtual void swap_buffers( void ) = 0;

    inline void
    viewport( coord_type x, coord_type y, coord_type w, coord_type h )
    {
        set_viewport( x, y, w, h );
    }
    inline void viewport( const rect &r )
    {
        set_viewport( r.x(), r.y(), r.width(), r.height() );
    }

    render_guard begin_render( void ) { return render_guard( this ); }

    clip_region_guard push_clip( const rect &r )
    {
        return clip_region_guard( this, r );
    }
    clip_region_guard
    push_clip( coord_type x, coord_type y, coord_type w, coord_type h )
    {
        return clip_region_guard( this, rect( x, y, w, h ) );
    }

    gl::api &api( void ) { return *_api; }

protected:
    friend class clip_region_guard;
    friend class render_guard;

    virtual void acquire( void ) = 0;
    virtual void release( void ) = 0;

    virtual void
    set_viewport( coord_type x, coord_type y, coord_type w, coord_type h ) = 0;

    virtual void push_clip_rect( const rect &r );
    virtual void reset_clip( const rect &r ) = 0;
    virtual void pop_clip_rect( void );

    rect             _cur_viewport;
    std::stack<rect> _cur_clip_stack;

    std::unique_ptr<gl::api> _api;

private:
};

} // namespace platform
