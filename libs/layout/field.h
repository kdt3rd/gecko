// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "layout.h"

namespace layout
{
////////////////////////////////////////

/// @brief Layout which vertically aligns two areas.
class field : public layout
{
public:
    field( const std::shared_ptr<area> &l, const std::shared_ptr<area> &e );

    coord field_minimum_width( void );

    void set_field_width( coord w ) { _width = w; }

    /// @brief Compute the minimum size of this layout.
    void compute_bounds( void ) override;

    /// @brief Compute the position and size of children
    void compute_layout( void ) override;

private:
    coord               _width = min_coord();
    std::weak_ptr<area> _label;
    std::weak_ptr<area> _field;
};

////////////////////////////////////////

} // namespace layout
