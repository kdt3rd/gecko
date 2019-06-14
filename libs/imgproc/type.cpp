// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#include "unifier.h"

#include <algorithm>

namespace
{
using namespace imgproc;

const std::type_index t_function( typeid( void( void ) ) );
const std::type_index t_uint8( typeid( uint8_t ) );
const std::type_index t_int8( typeid( int8_t ) );
const std::type_index t_uint16( typeid( uint16_t ) );
const std::type_index t_int16( typeid( int16_t ) );
const std::type_index t_uint32( typeid( uint32_t ) );
const std::type_index t_int32( typeid( int32_t ) );
const std::type_index t_uint64( typeid( uint64_t ) );
const std::type_index t_int64( typeid( int64_t ) );
const std::type_index t_float32( typeid( float ) );
const std::type_index t_float64( typeid( double ) );
const std::type_index t_boolean( typeid( bool ) );

class printer
{
public:
    printer( std::ostream &out ) : _out( out ) {}

    void operator()( const type_variable &t ) { _out << "#" << t.id(); }

    void operator()( const type_primary &t ) { _out << t.get_type(); }

    void operator()( const type_callable &t )
    {
        char c1 = '{', c2 = '}';
        if ( t.get_call_type() == type_callable::IMAGE )
        {
            c1 = '[';
            c2 = ']';
        }
        else if ( t.get_call_type() == type_callable::FUNCTION )
        {
            c1 = '(';
            c2 = ')';
        }

        _out << '*';
        if ( !t.empty() )
        {
            bool first = true;
            _out << c1;
            for ( auto i: t )
            {
                if ( !first )
                    _out << ',';
                _out << i;
            }
            _out << c2;
            _out << " -> ";
        }

        _out << t.get_result();
    }

    // Compile-time check for any missing operator() implementation
    template <typename T> void operator()( T )
    {
        static_assert(
            base::always_false<T>::value,
            "missing operator() for variant types" );
    }

    void visit( const type &t )
    {
        if ( t.valid() )
            base::visit( *this, t );
        else
            _out << "unknown";
    }

private:
    std::ostream &_out;
};

} // namespace

////////////////////////////////////////

namespace imgproc
{
////////////////////////////////////////

type_callable::type_callable( type result, call_type c )
    : _call( c ), _result( std::make_shared<type>( std::move( result ) ) )
{}

////////////////////////////////////////

void type_callable::add_arg( type t ) { _args.emplace_back( std::move( t ) ); }

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const type &t )
{
    printer p( out );
    p.visit( t );

    return out;
}

////////////////////////////////////////

} // namespace imgproc
