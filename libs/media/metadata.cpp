//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "metadata.h"
#include <mutex>
#include <vector>
#include <map>
#include <string>

////////////////////////////////////////

namespace media
{

class metadata_type
{
public:
    metadata_type( const char *name ) : _name( name ) {}
    metadata_type( const char *name, metadata_to_utf8_func toFunc, metadata_from_utf8_func fromFunc )
        : _name( name ), _serializer( toFunc ), _deserializer( fromFunc )
    {}

    const std::string &name( void ) const { return _name; }

    std::string _name;
    metadata_to_utf8_func _serializer = nullptr;
    metadata_from_utf8_func _deserializer = nullptr;
};

class metadata_type_store
{
public:
    metadata_type_store( void )
    {
        register_type( "void" );
        register_type( "group" );
        register_type( "list" );

        register_type( "int8" );
        register_type( "uint8" );
        register_type( "int16" );
        register_type( "uint16" );
        register_type( "int32" );
        register_type( "uint32" );
        register_type( "int64" );
        register_type( "uint64" );

        register_type( "srational32" );
        register_type( "urational32" );
        register_type( "rational32" );

        register_type( "float16" );
        register_type( "float32" );
        register_type( "float64" );

        register_type( "string" );
        register_type( "stringlist" );
        register_type( "u8string" );
        register_type( "u16string" );
        register_type( "u32string" );

        register_type( "vec2i" );
        register_type( "vec2f" );
        register_type( "vec2d" );
        register_type( "vec3i" );
        register_type( "vec3f" );
        register_type( "vec3d" );

        register_type( "recti" );
        register_type( "rectf" );

        register_type( "m33f" );
        register_type( "m33d" );
        register_type( "m44f" );
        register_type( "m44d" );

        register_type( "floatlist" );

        register_type( "chromaticities" );
        register_type( "keycode" );
        register_type( "timecode" );
    }

    uint32_t register_metadata_type(
        const char *name,
        metadata_to_utf8_func serialize,
        metadata_from_utf8_func deserialize
                                    )
    {
        std::lock_guard<std::mutex> lk( _mtype_mutex );
        auto i = _mtype_name_to_id.find( name );
        if ( i != _mtype_name_to_id.end() )
        {
            const metadata_type &mtype = *(_mtype_list[i->second]);
            if ( mtype._serializer == serialize && mtype._deserializer == deserialize )
                return i->second;
            throw_runtime( "Attempt to register duplicate metadata type name {0} with different serialization functions", name );
        }
        uint32_t id = _mtype_list.size();
        _mtype_list.emplace_back( std::make_shared<metadata_type>( name, serialize, deserialize ) );
        _mtype_name_to_id[base::cstring(_mtype_list.back()->name())] = id;
        return id;
    }

    uint32_t find_metadata_type( const char *name )
    {
        std::lock_guard<std::mutex> lk( _mtype_mutex );
        auto i = _mtype_name_to_id.find( name );
        if ( i != _mtype_name_to_id.end() )
            return i->second;
        return uint32_t(-1);
    }

    const char *metadata_type_name( uint32_t tag )
    {
        std::lock_guard<std::mutex> lk( _mtype_mutex );
        if ( tag < _mtype_list.size() )
            return _mtype_list[tag]->name().c_str();
        return nullptr;
    }

private:
    // TODO: add serialization...
    void register_type(
        const char *name,
        metadata_to_utf8_func toFunc = nullptr,
        metadata_from_utf8_func fromFunc = nullptr )
    {
        uint32_t id = _mtype_list.size();
        _mtype_list.emplace_back( std::make_shared<metadata_type>( name, toFunc, fromFunc ) );
        _mtype_name_to_id[base::cstring(_mtype_list.back()->name())] = id;
    }

    std::mutex _mtype_mutex;
    std::map<base::cstring, uint32_t> _mtype_name_to_id;
    std::vector< std::shared_ptr<metadata_type> > _mtype_list;
};

static metadata_type_store &get_store( void )
{
    static metadata_type_store the_store;
    return the_store;
}

uint32_t register_metadata_type(
    const char *name,
    metadata_to_utf8_func serialize,
    metadata_from_utf8_func deserialize
                                )
{
    return get_store().register_metadata_type( name, serialize, deserialize );
}

uint32_t find_metadata_type( const char *name )
{
    return get_store().find_metadata_type( name );
}

const char *metadata_type_name( uint32_t tag )
{
    return get_store().metadata_type_name( tag );
}

////////////////////////////////////////

} // media



