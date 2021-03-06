// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <base/const_string.h>
#include <base/small_vector.h>
#include <cstdint>
#include <string>

////////////////////////////////////////

namespace media
{
class image;
class data;

///
/// @brief Class view provides...
///
class view
{
public:
    view( base::cstring nm ) : _name( nm ) {}
    const std::string &name( void ) const { return _name; }

    /// the image for this view
    explicit operator const std::shared_ptr<image> &(void)const
    {
        return _image;
    }
    /// deep data or whatever other data channels a view has
    explicit operator const std::shared_ptr<data> &(void)const { return _data; }

    void store( const std::shared_ptr<image> &i ) { _image = i; }
    void store( const std::shared_ptr<data> &d ) { _data = d; }

private:
    std::string            _name;
    std::shared_ptr<image> _image;
    std::shared_ptr<data>  _data;
};

} // namespace media
