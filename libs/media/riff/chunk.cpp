// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "chunk.h"

namespace media
{
namespace riff
{
////////////////////////////////////////

chunk::chunk( std::istream &in )
{
    in >> _id;

    uint32_t s;
    in.read( reinterpret_cast<char *>( &s ), 4 );
    _size = s;

    _data = in.tellg();
}

////////////////////////////////////////

} // namespace riff
} // namespace media
