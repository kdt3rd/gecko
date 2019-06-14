// Copyright (c) 2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "area.h"

#include <list>
#include <memory>

namespace layout
{
////////////////////////////////////////

/// @brief Base class for a layout.
///
/// The base class provides padding (space around the outside of the children areas),
/// and spacing (space between children areas).
class layout : public area
{
public:
    ~layout( void ) override;

    /// @brief Set padding amount.
    void set_padding( coord l, coord r, coord t, coord b )
    {
        _pad[0] = l;
        _pad[1] = r;
        _pad[2] = t;
        _pad[3] = b;
    }

    /// @brief Set spacing amount.
    void set_spacing( coord h, coord v )
    {
        _spacing[0] = h;
        _spacing[1] = v;
    }

protected:
    /// @brief Distribute the extra width.
    void expand_width( std::list<std::shared_ptr<area>> &areas, coord extra );

    /// @brief Distribute the extra width.
    template <typename Container>
    void expand_width( const Container &areas, coord extra )
    {
        if ( extra <= min_coord() )
            return;

        // Lock the pointers and remove any null pointer.
        std::list<std::shared_ptr<area>> lst;
        for ( auto &p: areas )
        {
            auto a = p.lock();
            if ( a )
                lst.push_back( a );
        }

        expand_width( lst, extra );
    }

    /// @brief Distribute the extra height.
    void expand_height( std::list<std::shared_ptr<area>> &areas, coord extra );

    /// @brief Distribute the extra height.
    template <typename Container>
    void expand_height( const Container &areas, coord extra )
    {
        if ( extra <= min_coord() )
            return;

        // Lock the pointers and remove any null pointer.
        std::list<std::shared_ptr<area>> lst;
        for ( auto &p: areas )
        {
            auto a = p.lock();
            if ( a )
                lst.push_back( a );
        }

        expand_height( lst, extra );
    }

    coord _pad[4]     = { min_coord(), min_coord(), min_coord(), min_coord() };
    coord _spacing[2] = { min_coord(), min_coord() };
};

////////////////////////////////////////

} // namespace layout
