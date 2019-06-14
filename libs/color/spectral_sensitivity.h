// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "spectral_density.h"
#include "triplet.h"

#include <vector>

////////////////////////////////////////

namespace color
{
///
/// @brief Class spectral_sensitivity provides a generic abstraction
/// around producing tristimulus values from spectral density.
///
/// This can be used either by the color matching functions or by
/// camera spectral sensitivity measurements.
///
template <typename T> class spectral_sensitivity
{
public:
    using value_type = T;
    using table      = spectral_density<value_type>;

    spectral_sensitivity( void )                         = default;
    spectral_sensitivity( const spectral_sensitivity & ) = default;
    spectral_sensitivity( spectral_sensitivity && )      = default;
    spectral_sensitivity &operator=( const spectral_sensitivity & ) = default;
    spectral_sensitivity &operator=( spectral_sensitivity && ) = default;
    ~spectral_sensitivity( void )                              = default;

    spectral_sensitivity( table xb, table yb, table zb )
        : _x_bar( std::move( xb ) )
        , _y_bar( std::move( yb ) )
        , _z_bar( std::move( zb ) )
    {}

    value_type sample_x( value_type l ) const { return _x_bar.sample( l ); }
    value_type sample_x( value_type l, value_type d ) const
    {
        return _x_bar.sample( l, d );
    }
    value_type sample_y( value_type l ) const { return _y_bar.sample( l ); }
    value_type sample_y( value_type l, value_type d ) const
    {
        return _y_bar.sample( l, d );
    }
    value_type sample_z( value_type l ) const { return _z_bar.sample( l ); }
    value_type sample_z( value_type l, value_type d ) const
    {
        return _z_bar.sample( l, d );
    }

    template <typename OT>
    triplet<
        typename std::common_type<value_type, typename OT::value_type>::type>
    integrate( const spectral_density<OT> &d )
    {
        using rettype = typename std::
            common_type<value_type, typename OT::value_type>::type;
        return triplet<rettype>{ _x_bar.dot( d ),
                                 _y_bar.dot( d ),
                                 _z_bar.dot( d ) };
    }

    const table &x_bar( void ) const { return _x_bar; }
    const table &y_bar( void ) const { return _y_bar; }
    const table &z_bar( void ) const { return _z_bar; }

private:
    table _x_bar;
    table _y_bar;
    table _z_bar;
};

using spectral_sensitivity_f = spectral_sensitivity<float>;
using spectral_sensitivity_d = spectral_sensitivity<double>;

} // namespace color
