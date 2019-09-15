// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "screen.h"

namespace platform
{
////////////////////////////////////////

screen::screen( void ) {}

////////////////////////////////////////

screen::~screen( void ) {}

////////////////////////////////////////

phys_unit screen::to_physical_horiz( coord_type x )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    return phys_unit( static_cast<val_type>( x ) / d_mm.w() );
}

////////////////////////////////////////

phys_unit screen::to_physical_vert( coord_type y )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    return phys_unit( static_cast<val_type>( y ) / d_mm.h() );
}

////////////////////////////////////////

phys_point screen::to_physical( const point &p )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    return phys_point(
        phys_unit( static_cast<val_type>( p[0] ) / d_mm.w() ),
        phys_unit( static_cast<val_type>( p[1] ) / d_mm.h() ) );
}

////////////////////////////////////////

phys_size screen::to_physical( const size &p )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    return phys_size(
        phys_unit( static_cast<val_type>( p.w() ) / d_mm.w() ),
        phys_unit( static_cast<val_type>( p.h() ) / d_mm.h() ) );
}

////////////////////////////////////////

phys_rect screen::to_physical( const rect &p )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    return phys_rect(
        phys_unit( static_cast<val_type>( p.x() ) / d_mm.w() ),
        phys_unit( static_cast<val_type>( p.y() ) / d_mm.h() ),
        phys_unit( static_cast<val_type>( p.width() ) / d_mm.w() ),
        phys_unit( static_cast<val_type>( p.height() ) / d_mm.h() ) );
}

////////////////////////////////////////

coord_type screen::to_native_horiz( const phys_unit &u )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    using mm           = base::units::millimeters<val_type>;

    return static_cast<coord_type>( ( u * mm( d_mm.w() ) ).count() );
}

////////////////////////////////////////

coord_type screen::to_native_vert( const phys_unit &u )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    using mm           = base::units::millimeters<val_type>;

    return static_cast<coord_type>( ( u * mm( d_mm.h() ) ).count() );
}

////////////////////////////////////////

point screen::to_native( const phys_point &u )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    using mm           = base::units::millimeters<val_type>;

    return point(
        static_cast<coord_type>( ( u[0] * mm( d_mm.w() ) ).count() ),
        static_cast<coord_type>( ( u[1] * mm( d_mm.h() ) ).count() ) );
}

////////////////////////////////////////

size screen::to_native( const phys_size &u )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    using mm           = base::units::millimeters<val_type>;

    return size(
        static_cast<coord_type>(
            std::ceil( ( u.w() * mm( d_mm.w() ) ).count() ) ),
        static_cast<coord_type>(
            std::ceil( ( u.h() * mm( d_mm.h() ) ).count() ) ) );
}

////////////////////////////////////////

rect screen::to_native( const phys_rect &u )
{
    dots_per_unit d_mm = dpmm();
    using val_type     = dots_per_unit::coord_type;
    using mm           = base::units::millimeters<val_type>;

    return rect(
        static_cast<coord_type>( ( u.x() * mm( d_mm.w() ) ).count() ),
        static_cast<coord_type>( ( u.y() * mm( d_mm.h() ) ).count() ),
        static_cast<coord_type>(
            std::ceil( ( u.width() * mm( d_mm.w() ) ).count() ) ),
        static_cast<coord_type>(
            std::ceil( ( u.height() * mm( d_mm.h() ) ).count() ) ) );
}

////////////////////////////////////////

phys_rect screen::physical_bounds( bool avail )
{
    return to_physical( bounds( avail ) );
}

////////////////////////////////////////

} // namespace platform
