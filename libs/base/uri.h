// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "const_string.h"
#include "contract.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace base
{
////////////////////////////////////////

/// @brief The generalization of URL, URN, etc.
///
/// The defined parts are the following, with [] indicating optional sections
/// scheme:[//[user:password@]host[:port]][/]path[?query][#fragment]
class uri
{
public:
    uri( void ) {}

    uri( cstring str ) { parse( str ); }

    template <typename... Types>
    uri( cstring sch, cstring auth, Types... paths ) : _scheme( sch )
    {
        parse_authority( auth );
        add_paths( paths... );
    }

    template <typename... Types>
    uri( const uri &parent, Types... paths ) : uri( parent )
    {
        add_paths( paths... );
    }

    /// @brief Construct URI and hide the password if present.
    explicit uri( char *str );

    /// Can be used to set the scheme if the user has not provided one
    /// (i.e. with a file path).
    inline void set_scheme( cstring s ) { _scheme = s; }

    inline const std::string &scheme( void ) const { return _scheme; }

    const std::string &user( void ) const { return _user; }

    const std::string &host( void ) const { return _host; }

    uint16_t port( void ) const { return _port; }

    uint16_t port( uint16_t default_port ) const
    {
        if ( _port == 0 )
            return default_port;
        return _port;
    }

    const std::vector<std::string> &path( void ) const { return _path; }

    std::vector<std::string> &path( void ) { return _path; }

    const std::string &path( size_t i ) const { return _path.at( i ); }

    std::string full_path( void ) const;

    const std::string &query( void ) const { return _query; }

    std::vector<std::pair<std::string, std::string>>
    parse_query( char kv_sep = '=', char arg_sep = '&' ) const;

    const std::string &fragment( void ) const { return _fragment; }

    uri &operator=( cstring str )
    {
        parse( str );
        return *this;
    }

    uri &operator/=( cstring str )
    {
        add_path( str );
        return *this;
    }

    uri parent( void ) const
    {
        precondition( !_path.empty(), "no parent uri" );

        uri result( *this );
        result.path().pop_back();
        return result;
    }

    void add_path( cstring str );

    template <typename... Types> void add_paths( cstring str, Types... rest )
    {
        add_path( str );
        add_paths( rest... );
    }

    template <typename... Types>
    void add_paths( const std::vector<std::string> &paths, Types... rest )
    {
        for ( auto &p: paths )
            add_path( p );
        add_paths( rest... );
    }

    void add_paths( void ) {}

    uri root( void ) const;

    void split_query( std::vector<std::pair<std::string, std::string>> &q );

    std::string pretty( void ) const;

    explicit operator bool( void ) const { return !_scheme.empty(); }

    static std::string escape( cstring str );
    static std::string unescape( cstring str );

private:
    void parse( cstring str );
    void parse_authority( cstring str );

    std::string              _scheme;
    std::string              _user;
    std::string              _host;
    uint16_t                 _port = 0;
    std::vector<std::string> _path;
    // stash the raw query so we can later split the values prior to
    // unescaping so if a separator is encoded, it doesn't confuse
    std::string _raw_query;
    std::string _query;
    std::string _fragment;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const uri &u );

inline uri operator/( const uri &u, cstring str ) { return uri( u ) /= str; }

inline uri operator/( uri &&u, cstring str )
{
    u /= str;
    return std::move( u );
}

////////////////////////////////////////

} // namespace base
