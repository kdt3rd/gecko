// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "font_dpi_cache.h"

#include "font_manager.h"

#include <base/contract.h>

////////////////////////////////////////

namespace script
{
////////////////////////////////////////

font_dpi_cache::font_dpi_cache(
    font_manager *fm, int dpih, int dpiv, int mgw, int mgh )
    : _mgr( fm )
    , _dpi_h( dpih )
    , _dpi_v( dpiv )
    , _max_glyph_w( mgw )
    , _max_glyph_h( mgh )
{}

////////////////////////////////////////

font_dpi_cache::~font_dpi_cache( void ) {}

////////////////////////////////////////

std::shared_ptr<font_dpi_cache>
font_dpi_cache::make( font_manager *fm, int dpih, int dpiv, int mgw, int mgh )
{
    struct rebind_dpi_cache : public font_dpi_cache
    {
        rebind_dpi_cache(
            font_manager *fm, int dpih, int dpiv, int mgw, int mgh )
            : font_dpi_cache( fm, dpih, dpiv, mgw, mgh )
        {}
    };
    return std::make_shared<rebind_dpi_cache>( fm, dpih, dpiv, mgw, mgh );
}

////////////////////////////////////////

std::shared_ptr<font> font_dpi_cache::get_font(
    const std::string &family, const std::string &style, points pts )
{
    precondition(
        _mgr, "font_dpi_cache somehow constructed with null font manager" );
    std::lock_guard<std::mutex> lk( _mx );
    auto &                      centry = _cache[family][style][pts];
    if ( !centry )
        centry = _mgr->get_font(
            family, style, pts, _dpi_h, _dpi_v, _max_glyph_w, _max_glyph_h );
    return centry;
}

////////////////////////////////////////

} // namespace script
