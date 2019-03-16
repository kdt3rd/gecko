// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "selection.h"

////////////////////////////////////////
namespace platform
{

////////////////////////////////////////

selection::selection( selection_type st, const std::string &customSelType )
    : _type( st ), _custom_selection_name( customSelType )
{}

////////////////////////////////////////

selection::selection(
    selection_type st, const std::string &s, const std::string &customSelType )
    : _data( s.begin(), s.end() )
    , _data_type( "text/plain;charset=utf-8" )
    , _type( st )
    , _custom_selection_name( customSelType )
{}

////////////////////////////////////////

selection::selection(
    selection_type st,
    const data_container &data,
    const std::string &mime_type,
    const std::vector<std::string> &avail_mime,
    const mime_converter &converter,
    const std::string &customSelType )
    : _data( data )
    , _data_type( mime_type )
    , _avail_mime( avail_mime )
    , _convert_func( converter )
    , _type( st )
    , _custom_selection_name( customSelType )
{}

////////////////////////////////////////

selection::~selection( void ) {}

////////////////////////////////////////

const std::string &selection::current_mime_type( void ) const
{
    return ( _cache_type.empty() ) ? _data_type : _cache_type;
}

////////////////////////////////////////

const std::vector<uint8_t> &selection::as( const std::string &mime_type )
{
    if ( mime_type == _data_type )
        return _data;
    if ( mime_type == _cache_type )
        return _convert_cache;

    if ( _convert_func )
    {
        _convert_cache = _convert_func( _data, _data_type, mime_type );
        _cache_type = mime_type;
        return _convert_cache;
    }

    static std::vector<uint8_t> kEmpty;
    return kEmpty;
}

////////////////////////////////////////

void selection::clear( void )
{
    _data.clear();
    _data.shrink_to_fit();
    _data_type.clear();
    _convert_cache.clear();
    _convert_cache.shrink_to_fit();
    _cache_type.clear();

    _avail_mime.clear();
    _convert_func = mime_converter();

    _type = selection_type::MOUSE;
    _custom_selection_name.clear();
}

} // namespace platform
