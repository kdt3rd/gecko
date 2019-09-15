// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "token.h"
#include "type.h"

#include <vector>

namespace imgproc
{
////////////////////////////////////////

class decl
{
public:
    decl( std::u32string n ) : _name( std::move( n ) ) {}

    decl( void ) {}

    void set_name( std::u32string n ) { _name = std::move( n ); }

    void add_arg( type t ) { _type.add_arg( t ); }

    const std::u32string &name( void ) const { return _name; }

    void parse( iterator &token );

    const type_callable &get_type( void ) const { return _type; }

private:
    type parse_type( iterator &token );

    std::u32string _name;
    type_callable  _type;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const decl &d );

////////////////////////////////////////

} // namespace imgproc
