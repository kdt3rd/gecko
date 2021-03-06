// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "extents.h"

#include <map>
#include <memory>
#include <mutex>

////////////////////////////////////////

namespace script
{
class font;
class font_manager;

///
/// @brief Class font_dpi_cache provides a cache for a specific dpi
///
/// since each font instance needs it's own dpi, create a cache for
/// each dpi that the font manager can provide
///
class font_dpi_cache
{
public:
    ~font_dpi_cache( void );

    std::shared_ptr<font>
    get_font( const std::string &family, const std::string &style, points pts );

private:
    friend class script::font_manager;
    static std::shared_ptr<font_dpi_cache>
    make( font_manager *fm, float dpih, float dpiv, int mgw, int mgh );

    font_dpi_cache(
        font_manager *fm, float dpih, float dpiv, int mgw, int mgh );

    font_dpi_cache( const font_dpi_cache & ) = delete;
    font_dpi_cache( font_dpi_cache && )      = delete;
    font_dpi_cache &operator=( const font_dpi_cache & ) = delete;
    font_dpi_cache &operator=( font_dpi_cache && ) = delete;

    font_manager *_mgr         = nullptr;
    float         _dpi_h       = 95.f;
    float         _dpi_v       = 95.f;
    int           _max_glyph_w = 1024;
    int           _max_glyph_h = 1024;

    using pt_cache    = std::map<points, std::shared_ptr<font>>;
    using style_cache = std::map<std::string, pt_cache>;
    using cache_type  = std::map<std::string, style_cache>;

    std::mutex _mx;
    cache_type _cache;
};

} // namespace script
