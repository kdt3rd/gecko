// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "sample_data.h"
#include "sample_rate.h"

#include <memory>
#include <ostream>
#include <utility>

////////////////////////////////////////

namespace media
{
///
/// @brief Class sample provides...
///
class sample
{
public:
    inline sample( void ) = default;
    inline sample( int64_t o, const sample_rate &sr )
        : _offset( o ), _rate( sr )
    {}
    inline ~sample( void )          = default;
    inline sample( const sample & ) = default;
    inline sample &operator=( const sample & ) = default;
    inline sample( sample && )                 = default;
    inline sample &operator=( sample && ) = default;

    inline int64_t            offset( void ) const { return _offset; }
    inline const sample_rate &rate( void ) const { return _rate; }

    template <typename T>
    typename std::shared_ptr<typename std::remove_pointer<
        decltype( std::declval<T>().read( int64_t(), sample_rate() ) )>::type>
    operator()( const std::shared_ptr<T> &track ) const
    {
        typedef decltype(
            std::declval<T>().read( int64_t(), sample_rate() ) ) ret_ptr;
        typedef typename std::remove_pointer<ret_ptr>::type      ret_type;
        static_assert(
            std::is_base_of<sample_data, ret_type>::value,
            "Track object read value no derived from sample_data" );
        return std::shared_ptr<ret_type>( track->read( _offset, _rate ) );
    }

private:
    int64_t     _offset = -1;
    sample_rate _rate;
};

////////////////////////////////////////

inline bool operator==( const sample &a, const sample &b )
{
    if ( a.rate() == b.rate() )
        return a.offset() == b.offset();

    sample_rate com = a.rate().common( b.rate() );
    return com.resample( a.offset(), a.rate() ) ==
           com.resample( b.offset(), b.rate() );
}

inline bool operator!=( const sample &a, const sample &b )
{
    return !( a == b );
}

inline bool operator<( const sample &a, const sample &b )
{
    if ( a.rate() == b.rate() )
        return a.offset() < b.offset();

    sample_rate com = a.rate().common( b.rate() );
    return com.resample( a.offset(), a.rate() ) <
           com.resample( b.offset(), b.rate() );
}

inline bool operator<=( const sample &a, const sample &b )
{
    if ( a.rate() == b.rate() )
        return a.offset() <= b.offset();

    sample_rate com = a.rate().common( b.rate() );
    return com.resample( a.offset(), a.rate() ) <=
           com.resample( b.offset(), b.rate() );
}

inline bool operator>( const sample &a, const sample &b ) { return b < a; }

inline bool operator>=( const sample &a, const sample &b ) { return b <= a; }

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &os, const sample &s )
{
    os << s.offset() << '@' << s.rate();
    return os;
}

} // namespace media
