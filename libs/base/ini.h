// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////

namespace base
{
/// @brief a grouping of key/value pairs belonging to an ini file
class ini_section
{
public:
    ini_section( void ) = default;
    explicit ini_section( const std::string &name );

    const std::string &name( void ) const { return _name; }

    bool empty( void ) const { return _values.empty(); }

    void parse( std::string v );

    template <typename K> bool has_key( const K &k ) const
    {
        auto i = _keys.find( k );
        if ( i != _keys.end() )
            return true;
    }

    template <typename K>
    std::string
    defaulted_value( const K &k, std::string defaultVal = std::string() ) const
    {
        auto i = _keys.find( k );
        if ( i != _keys.end() )
            return _values[i->second].second;
        return defaultVal;
    }

    template <typename K> const std::string &operator[]( const K &k ) const
    {
        auto i = _keys.find( k );
        if ( i != _keys.end() )
            return _values[i->second].second;

        static std::string empty;
        return empty;
    }

    void save( std::ostream &os ) const;

private:
    std::string                                      _name;
    std::map<std::string, size_t>                    _keys;
    std::vector<std::pair<std::string, std::string>> _values;
};

///
/// @brief Class ini provides an ini-file parser
///
/// This parses the venerable ini file format such as the below:
///
/// foo=bar
///
/// [section]
/// bar=baz
///
///
class ini
{
public:
    const ini_section &globals( void ) const { return _globals; }

    template <typename K> bool has_section( const K &k ) const
    {
        auto i = _sections.find( k );
        if ( i != _sections.end() )
            return true;
        return false;
    }

    template <typename K> const ini_section &operator[]( const K &k ) const
    {
        auto i = _sections.find( k );
        if ( i != _sections.end() )
            return *( i->second );

        return _globals;
    }

    void parse( std::istream &is );
    void save( std::ostream &os ) const;

    static ini load( std::istream &is );

private:
    ini_section                                         _globals;
    std::map<std::string, std::shared_ptr<ini_section>> _sections;
    std::vector<std::string>                            _sec_order;
};

} // namespace base
