// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "frame.h"
#include "sample.h"
#include "sample_data.h"
#include "sample_rate.h"
#include "track_description.h"

#include <functional>
#include <memory>
#include <string>

namespace media
{
////////////////////////////////////////

/// @brief Base class for all tracks in a media file
///
/// This can be multiple files (i.e. image files on disk), or a single
/// sub-track of a container file (i.e. quicktime, avi, etc.).
/// Sample track types:
///  - Video Track
///  - Audio Track
///  - (Timed) Data Track (track of data, each data chunk attached to a time)
///    - Timed Text Track (subtitles)
///    - XMP data per frame
class track
{
public:
    // TODO: b, e, r are duplicate w/ info in track_description
    track(
        std::string              n,
        std::string              v,
        int64_t                  b,
        int64_t                  e,
        const sample_rate &      r,
        const track_description &td )
        : _name( std::move( n ) )
        , _view( std::move( v ) )
        , _desc( td )
        , _begin( b )
        , _end( e )
        , _rate( r )
    {}

    virtual ~track( void );

    inline const std::string &name( void ) const;
    inline const std::string &view( void ) const;
    inline int64_t            begin( void ) const;
    inline int64_t            end( void ) const;
    inline const sample_rate &rate( void ) const;

    inline const track_description &desc( void ) const { return _desc; }

protected:
    friend class sample;
    // we will use covariance to access the return types for sub classes of this
    virtual sample_data *read( int64_t offset, const sample_rate &r ) = 0;
    //	// we will use covariance to access the return types for sub classes of this
    //	virtual void write( int64_t offset, const sample_rate &r, const sample_data &sd ) = 0;

    void update_rate( const sample_rate &r );

private:
    std::string       _name;
    std::string       _view;
    track_description _desc;
    int64_t           _begin, _end;
    sample_rate       _rate;
};

////////////////////////////////////////

inline const std::string &track::name( void ) const { return _name; }
inline const std::string &track::view( void ) const { return _view; }
inline int64_t            track::begin( void ) const { return _begin; }
inline int64_t            track::end( void ) const { return _end; }
inline const sample_rate &track::rate( void ) const { return _rate; }
inline void track::update_rate( const sample_rate &r ) { _rate = r; }

////////////////////////////////////////

} // namespace media
