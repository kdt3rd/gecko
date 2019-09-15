// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <base/uri.h>
#include <iostream>

namespace media
{
////////////////////////////////////////

class file_sequence
{
public:
    enum marker_style
    {
        MARKER_POUND,
        MARKER_PERCENTD,
        MARKER_DOLLARF,
        MARKER_AUTO
    };

    file_sequence( base::uri path, marker_style ms = MARKER_AUTO );

    inline const base::uri &parent( void ) const;

    base::uri uri( void ) const;

    base::uri get_frame( int64_t f ) const;

    /// returns false if the provided uri does not match the
    /// URI in the current
    bool extract_frame( const base::uri &u, int64_t &f ) const;

    static file_sequence
    guess( base::uri path, marker_style ms = MARKER_POUND );

private:
    bool extract_pound_pattern( void );
    bool extract_dollarf_pattern( void );
    bool extract_percentd_pattern( void );
    void split_pattern( size_t start, size_t sz );

    base::uri _uri;

    std::string  _pattern_prefix;
    std::string  _pattern_suffix;
    std::string  _pattern;
    marker_style _marker_style = MARKER_AUTO;
    size_t       _pattern_size = 0;
};

inline std::ostream &operator<<( std::ostream &os, const file_sequence &fs )
{
    os << fs.uri();
    return os;
}

////////////////////////////////////////

inline const base::uri &file_sequence::parent( void ) const { return _uri; }

////////////////////////////////////////

} // namespace media
