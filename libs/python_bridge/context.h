// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <base/uri.h>
#include <cstdint>
#include <memory>
#include <string>

////////////////////////////////////////

namespace python_bridge
{
class lookup;

class context
{
public:
    context( const std::string &argv0 );
    ~context( void );
    context( const context & ) = delete;
    context( context && )      = delete;
    context &operator=( const context & ) = delete;
    context &operator=( context && ) = delete;

    bool run( const std::string &fn );
    bool run( const base::uri &u );

    /// pushes a lookup object to allow object / variable lookup
    void push_eval_lookup( lookup &l );
    void pop_eval_lookup( void );

    bool evaluate( bool &result, const char *s );
    bool evaluate( int &result, const char *s );
    bool evaluate( float &result, const char *s );
    bool evaluate( double &result, const char *s );
    bool evaluate( int64_t &result, const char *s );
    bool evaluate( std::string &result, const char *s );
    bool evaluate( std::wstring &result, const char *s );

private:
#ifdef HAVE_PYTHON
    class priv_Impl;
    std::unique_ptr<priv_Impl> _impl;
#endif
};

} // namespace python_bridge
