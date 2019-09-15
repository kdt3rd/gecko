// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <utf/utf.h>

namespace imgproc
{
////////////////////////////////////////

class location
{
public:
    void set( const utf::iterator &it )
    {
        _line = it.line_number();
        _char = it.char_number();
    }

    size_t line_number( void ) const { return _line; }
    size_t char_number( void ) const { return _char; }

private:
    size_t _line = 0;
    size_t _char = 0;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const location &l )
{
    out << "line " << l.line_number() << " @ " << l.char_number();
    return out;
}

////////////////////////////////////////

} // namespace imgproc
