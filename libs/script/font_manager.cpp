// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "font_manager.h"

namespace script
{
////////////////////////////////////////

font_manager::font_manager( void ) {}

////////////////////////////////////////

font_manager::~font_manager( void ) {}

////////////////////////////////////////

std::shared_ptr<font_dpi_cache>
font_manager::get_cache( int dpih, int dpiv, int maxGlyphW, int maxGlyphH )
{
    std::pair<int, int> id = std::make_pair( dpih, dpiv );

    std::shared_ptr<font_dpi_cache> ret;

    std::lock_guard<std::mutex> lk( _mx );
    auto                        i = _dpi_cache.find( id );
    if ( i == _dpi_cache.end() )
    {
        ret = font_dpi_cache::make( this, dpih, dpiv, maxGlyphW, maxGlyphH );
        _dpi_cache[id] = ret;
    }
    else
        ret = i->second;
    return ret;
}

////////////////////////////////////////

} // namespace script
