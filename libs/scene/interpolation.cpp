// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "interpolation.h"

#include <base/contract.h>

////////////////////////////////////////

namespace scene
{

namespace
{
template <typename eval> class interp_impl : public interpolation
{
public:
    using calc_type = interpolation::calc_type;

    interp_impl( void )           = default;
    ~interp_impl( void ) override = default;

    calc_type
    offset( const time &start, const time &end, const time &t ) const override
    {
        return eval::offset( start, end, t );
    }

    size_t storage_size( void ) const override { return eval::storage_size; }

    size_t control_points( void ) const override
    {
        return eval::control_points;
    }
};

template <interp_style> struct interp_eval
{};

template <> struct interp_eval<interp_style::constant>
{
    static constexpr size_t storage_size   = 0;
    static constexpr size_t control_points = 0;

    static constexpr interpolation::calc_type
    offset( const time &, const time &, const time & )
    {
        return interpolation::calc_type( 0 );
    }
};

using constant_impl = interp_impl<interp_eval<interp_style::constant>>;

} // namespace

////////////////////////////////////////

interpolation::interpolation( void ) {}

////////////////////////////////////////

interpolation::~interpolation( void ) {}

////////////////////////////////////////

std::unique_ptr<interpolation> interpolation::create( interp_style i )
{
    switch ( i )
    {
        case interp_style::constant:
            return std::unique_ptr<interpolation>( new constant_impl );

        default: break;
    }
    throw_logic( "Specified interpolation style not yet implemented" );
}

} // namespace scene
