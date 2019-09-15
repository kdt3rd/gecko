// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "fourcc.h"

#include <base/contract.h>
#include <cstdint>
#include <iostream>

namespace media
{
namespace riff
{
#if 0
template <typename crtp_child>
class base_chunk
{
	const fourcc &id( void ) const
	{
		return static_cast<const crtp_child *>(this)->_id;
	}

	size_t size( void ) const
	{
		return static_cast<const crtp_child *>(this)->_size;
	}

	std::istream &seek( std::istream &in )
	{
		in.seekg( _data );
		return in;
	}

	
};

template <typename Length, bool length_first = false, bool includes_crc32_at_end = false, bool size_is_data_only = true>
class chunk
{
};

template <>
class chunk<size_t, false, false> : public base_chunk< chunk<size_t, false, false> >
{
public:
	using chunk_size_type = size_t;

private:
	using base = base_chunk< chunk<size_t, false, false> >;
	friend class base_chunk< chunk<size_t, false, false> >;
	fourcc _id;
	chunk_size_type _size;
};
#endif

////////////////////////////////////////

class chunk
{
public:
    chunk( std::istream &in );

    const fourcc &id( void ) const { return _id; }

    size_t size( void ) const { return _size; }

    std::istream &seek( std::istream &in )
    {
        in.seekg( _data );
        return in;
    }

private:
    fourcc                 _id;
    size_t                 _size;
    std::istream::pos_type _data;
};

////////////////////////////////////////

} // namespace riff
} // namespace media
