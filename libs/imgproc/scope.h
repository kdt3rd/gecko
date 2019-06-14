// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "variable.h"

#include <map>
#include <memory>
#include <utf/utf.h>

namespace imgproc
{
////////////////////////////////////////

class scope
{
public:
    scope( void ) {}

    scope( const std::shared_ptr<scope> &p )
        : _parent( p ), _compile( p->_compile )
    {}

    void set_compile( const std::function<data_type(
                          const std::shared_ptr<function> &,
                          const std::vector<data_type> & )> &cmp )
    {
        _compile = cmp;
    }

    const variable &get( const std::u32string &n )
    {
        auto v = _variables.find( n );
        if ( v == _variables.end() )
        {
            if ( _parent )
                return _parent->get( n );
            else
                throw_runtime( "variable \"{0}\" not found", n );
        }
        return v->second;
    }

    const std::shared_ptr<function> &get_function( const std::u32string &n )
    {
        auto f = _functions.find( n );
        if ( f == _functions.end() )
        {
            if ( _parent )
                return _parent->get_function( n );
            else
                throw_runtime( "function \"{0}\" not found", n );
        }
        return f->second;
    }

    void add( const std::u32string &n, const data_type &t )
    {
        _variables.emplace( std::make_pair( n, variable( n, t ) ) );
    }

    void add_function(
        const std::u32string & /*n*/, const std::shared_ptr<function> & /*f*/ )
    {
        // TODO
        /*
		add( n, { pod_type::FUNCTION, 0 } );
		_functions.emplace( std::make_pair( n, f ) );
		*/
    }

    data_type compile(
        const std::shared_ptr<function> &f, const std::vector<data_type> &args )
    {
        return _compile( f, args );
    }

private:
    std::shared_ptr<scope>                              _parent;
    std::map<std::u32string, variable>                  _variables;
    std::map<std::u32string, std::shared_ptr<function>> _functions;
    std::function<data_type(
        const std::shared_ptr<function> &, const std::vector<data_type> & )>
        _compile;
};

////////////////////////////////////////

} // namespace imgproc
