// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

// not strictly necessary, but add here as a convenience
#include "font.h"
#include "font_dpi_cache.h"

namespace script
{
////////////////////////////////////////

/// @brief Font manager
///
/// Allows creation and listing of fonts.
class font_manager
{
public:
    /// @brief Constructor.
    font_manager( void );

    /// @brief Destructor.
    virtual ~font_manager( void );

    /// @brief List of families.
    ///
    /// Get a list of families of all managed fonts.
    /// @return The list of families
    virtual std::set<std::string> get_families( void ) = 0;

    /// @brief List of style.
    ///
    /// Get a list of styles available.
    /// @return The list of styles.
    virtual std::set<std::string> get_styles( const std::string &family ) = 0;

    /// @brief Retrieves a reference to a cache for a specific DPI
    ///
    /// this can be used as a convenience reference such that there is
    /// no need to carry the DPI for a screen or whatever else is
    /// going on, providing a simpler wrapper around
    /// @sa get_font. However, it's use is not required.
    ///
    /// NB: the max glyph width and height is currently considered
    /// auxiliary data that goes with the dpi. This might be different
    /// if there are two windows running on different screens that
    /// then each have their own font manager for different DPI, and
    /// the screens have different maximum texture sizes.
    ///
    /// if this is being used where openGL or whatever drawing kit is
    /// being used isn't constrained, pass in a suitable maximum glyph
    /// size for memory cache.
    std::shared_ptr<font_dpi_cache>
    get_cache( int dpih, int dpiv, int maxGlyphW, int maxGlyphH );

    /// @brief Get a particular font.
    ///
    /// Create and return a font with the given attributes.
    /// @param family Family of the font to create
    /// @param style Style of the font to create
    /// @param pts Size of the font in points to create
    /// @param dpih horizontal DPI used when creating font
    /// @param dpiv vertical DPI used when creating font
    /// @param maxGlyphW maximum glyph cache width
    /// @param maxGlyphH maximum glyph cache height
    ///
    /// @return The best matching font found
    virtual std::shared_ptr<font> get_font(
        const std::string &family,
        const std::string &style,
        points             pts,
        int                dpih,
        int                dpiv,
        int                maxGlyphW,
        int                maxGlyphH ) = 0;

    /// @brief Create an instance of a font manager
    ///
    /// This is custom per operating system type, and creates an
    /// instance of a font manager for general use. Multiple font
    /// managers may be in existence, although from a cache
    /// perspective, is discouraged. Each O.S. is responsible for
    /// maintaining it's own data structure to hide any required
    /// singletons.
    static std::shared_ptr<font_manager> make( void );

protected:
    std::mutex                                                     _mx;
    std::map<std::pair<int, int>, std::shared_ptr<font_dpi_cache>> _dpi_cache;
};

////////////////////////////////////////

} // namespace script
