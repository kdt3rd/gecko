
#pragma once

#include "frame.h"
#include "sample_rate.h"
#include "sample.h"

#include <string>
#include <functional>
#include <memory>

namespace media
{

////////////////////////////////////////


/// @brief Base class for all tracks in a media file
///
/// This can be multiple files (i.e. image files on disk), or a single
/// track of a container file (i.e. quicktime, avi, etc.).
/// Sample track types:
///  - Video Track
///  - Audio Track
///  - Timed Data Track (track of data, each data chunk attached to a time)
///    - Timed Text Track (subtitles)
///    - XMP data per frame
class track
{
public:
	track( std::string n, int64_t b, int64_t e, const sample_rate &r )
		: _name( std::move( n ) ), _begin( b ), _end( e ), _rate( r )
	{
	}

	virtual ~track( void );

	inline const std::string &name( void ) const;
	inline int64_t begin( void ) const;
	inline int64_t end( void ) const;
	inline const sample_rate &rate( void ) const;

//	virtual std::shared_ptr<sample_data> read( const sample &s ) = 0;
//	virtual void store( const std::shared_ptr<sample_data> &s, int64_t offset ) = 0;
	
private:
	std::string _name;
	int64_t _begin, _end;
	sample_rate _rate;
};

////////////////////////////////////////

inline const std::string &track::name( void ) const { return _name; }
inline int64_t track::begin( void ) const { return _begin; }
inline int64_t track::end( void ) const { return _end; }
inline const sample_rate &track::rate( void ) const { return _rate; }

////////////////////////////////////////

}

