// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

/// @cond UTF

#pragma once

#include <iostream>
#include <string>

namespace xml
{
////////////////////////////////////////

class reader
{
public:
    virtual ~reader( void );

    virtual void begin_element( const std::string &name ) = 0;
    virtual void content( const std::string &value )      = 0;
    virtual void
                 attribute( const std::string &name, const std::string &value ) = 0;
    virtual void end_element( const std::string &name ) = 0;

    void parse( std::istream &stream );
};

////////////////////////////////////////

// Read operator for XML nodes
inline std::istream &operator>>( std::istream &stream, reader &reader )
{
    reader.parse( stream );
    return stream;
}

////////////////////////////////////////

} // namespace xml

/// @endcond
