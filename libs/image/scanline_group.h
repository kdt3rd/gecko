// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "scanline.h"

#include <vector>

////////////////////////////////////////

namespace image
{
///
/// @brief Class scanline_group provides...
///
class scanline_group
{
public:
    explicit scanline_group( int offx, int w, size_t nOuts );
    ~scanline_group( void );

    void     output_scan( size_t i, scanline &&s );
    scanline output_scan_and_clear( size_t i );

    // if the number of outputs is > 1, this will never return an output scanline
    scanline
    find_or_checkout( const std::vector<scanline> &inputs, bool in_place );

private:
    std::vector<scanline> _outputs;
    std::vector<scanline> _spare;
    int                   _scan_offset = 0;
    int                   _scan_width  = 0;
};

} // namespace image
