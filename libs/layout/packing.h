// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "layout.h"

#include <base/alignment.h>
#include <list>
#include <memory>

namespace layout
{
////////////////////////////////////////

/// @brief Layout which packs areas arounds the sides of a shrinking center area.
///
/// This layout puts it's areas along the sides in the order added.
/// The widget will fill the selectd side, but will be at minimum size in the orthogonal direction.
/// The last area can be put in the center, taking the rest of the unused space.
class packing : public layout
{
public:
    /// @brief Default constructor.
    packing( void );

    /// @brief Add area to one side.
    //
    /// @param a Area to add.
    /// @param where Which side to add to.
    void add( const std::shared_ptr<area> &a, alignment where );

    void compute_bounds( void ) override;

    void compute_layout( void ) override;

private:
    struct section
    {
        section( const std::shared_ptr<area> &ar, alignment al )
            : _area( ar ), _align( al )
        {}

        std::weak_ptr<area> _area;
        alignment           _align;
    };

    std::list<section> _areas;
};

////////////////////////////////////////

} // namespace layout
