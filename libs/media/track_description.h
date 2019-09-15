// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "metadata.h"
#include "sample_rate.h"

#include <base/const_string.h>

////////////////////////////////////////

namespace media
{
enum track_type
{
    TRACK_VIDEO,
    TRACK_AUDIO,
    TRACK_DATA
};

///
/// @brief Class track_description provides the information necessary
///        to create a new output track. These are combined together to make
///        a new container.
///
class track_description
{
public:
    track_description( track_type tt );

    inline track_type type( void ) const { return _type; }

    inline void               name( base::cstring n ) { _name = n; }
    inline const std::string &name( void ) const { return _name; }

    inline void               view( base::cstring n ) { _view = n; }
    inline const std::string &view( void ) const { return _view; }

    inline void    offset( int64_t o ) { _offset = o; }
    inline int64_t offset( void ) const { return _offset; }
    inline void    duration( int64_t num_samples ) { _duration = num_samples; }
    inline int64_t duration( void ) const { return _duration; }
    inline void    rate( const sample_rate &sr ) { _sample_rate = sr; }
    const sample_rate &rate( void ) const { return _sample_rate; }

    inline void               codec( base::cstring name ) { _codec = name; }
    inline const std::string &codec( void ) const { return _codec; }

    inline void set_option( base::cstring opt, metadata_value v )
    {
        _options[opt] = std::move( v );
    }
    inline const metadata &options( void ) const { return _options; }

    inline void set_meta( base::cstring name, metadata_value v )
    {
        _metadata[name] = std::move( v );
    }
    inline const metadata &meta( void ) const { return _metadata; }

private:
    track_type  _type;
    std::string _name;
    std::string _view;
    int64_t     _offset   = -1;
    int64_t     _duration = -1;
    sample_rate _sample_rate;

    std::string _codec;
    metadata    _options;

    metadata _metadata;
};

} // namespace media
