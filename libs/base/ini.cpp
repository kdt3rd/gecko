// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "ini.h"

#include "contract.h"

#include <istream>
#include <ostream>

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

ini_section::ini_section( const std::string &name ) : _name( name )
{}

////////////////////////////////////////

void ini_section::parse( std::string v )
{
    // just store off comment fields
    // technically, # may not be a comment in ini files, but there's no standard...
    if ( v.front() == ';' || v.front() == '#' )
        _values.emplace_back( std::move( v ), std::string() );
    else
    {
        std::string::size_type eq = v.find_first_of( '=' );
        if ( eq != std::string::npos )
        {
            std::string val, key;
            key        = v.substr( 0, eq );
            val        = v.substr( eq + 1 );
            _keys[key] = _values.size();
            _values.emplace_back( std::move( key ), std::move( val ) );
        }
    }
}

////////////////////////////////////////

void ini_section::save( std::ostream &os ) const
{
    if ( !_name.empty() )
        os << '[' << _name << "]\n";
    for ( const auto &i: _values )
    {
        auto iskv = _keys.find( i.first );
        if ( iskv != _keys.end() )
            os << i.first << '=' << i.second << '\n';
        else
        {
            precondition( i.second.empty(), "expect only a single string" );
            os << i.first << '\n';
        }
    }
    if ( !_values.empty() )
        os << '\n';
}

////////////////////////////////////////

void ini::parse( std::istream &is )
{
    ini_section *curSec = &_globals;
    std::string  curl;
    while ( std::getline( is, curl ) )
    {
        std::string::size_type fc = 0;
        while ( fc < curl.size() && ( curl[fc] == ' ' || curl[fc] == '\t' ) )
            ++fc;

        if ( fc < curl.size() )
        {
            if ( curl[fc] == '[' )
            {
                // new section start...
                ++fc;
                std::string::size_type secend = curl.find_first_of( ']', fc );
                if ( secend == std::string::npos )
                    throw_runtime(
                        "Missing close bracket in section name '{0}'", curl );

                std::string secn = curl.substr( fc, secend - fc );

                precondition(
                    _sections.find( secn ) == _sections.end(),
                    "parser not written to merge duplicate sections" );
                auto i = _sections.emplace(
                    secn, std::make_shared<ini_section>( secn ) );
                curSec = i.first->second.get();
                // store the order so we can preserve the order for users and not confuse them
                _sec_order.push_back( secn );
            }
            else
                curSec->parse( curl.substr( fc ) );
        }
    }
}

////////////////////////////////////////

void ini::save( std::ostream &os ) const
{
    _globals.save( os );
    for ( const auto &i: _sec_order )
    {
        auto s = _sections.find( i );
        postcondition( s != _sections.end(), "section missing" );
        s->second->save( os );
    }
}

////////////////////////////////////////

ini ini::load( std::istream &is )
{
    ini ret;
    ret.parse( is );
    return ret;
}

////////////////////////////////////////

} // namespace base
