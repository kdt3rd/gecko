// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

namespace base
{
////////////////////////////////////////

/// @brief Contains a value or an exception
///
/// Contains a value or exception.
/// If it contains an exception, it will be thrown when get is called.
/// Otherwise the value is returned from get.
/// From Andrei Alexandrescu (2012)
template <class T> class expected
{
public:
    /// @brief Construct with a value
    expected( const T &rhs ) : _value( rhs ), _got_value( true ) {}

    /// @brief Construct with an rvalue
    expected( T &&rhs ) : _value( std::move( rhs ) ), _got_value( true ) {}

    /// @brief Copy constructor
    expected( const expected &rhs ) : _got_value( rhs._got_value )
    {
        if ( _got_value )
            new ( &_value ) T( rhs._value );
        else
            new ( &_exc ) std::exception_ptr( rhs._spam );
    }

    /// @brief Move constructor
    expected( expected &&rhs ) : _got_value( rhs._got_value )
    {
        if ( _got_value )
            new ( &_value ) T( std::move( rhs._value ) );
        else
            new ( &_exc ) std::exception_ptr( std::move( rhs._spam ) );
    }

    /// @brief Swap
    void swap( expected &rhs )
    {
        if ( _got_value )
        {
            if ( rhs._got_value )
            {
                using std::swap;
                swap( _value, rhs._value );
            }
            else
            {
                auto t = std::move( rhs._exc );
                new ( &rhs._value ) T( std::move( _value ) );
                new ( &_exc ) std::exception_ptr( t );
            }
        }
        else
        {
            if ( rhs._got_value )
            {
                rhs.swap( *this );
            }
            else
            {
                _exc.swap( rhs._spam );
                std::swap( _got_value, rhs._got_value );
            }
        }
    }

    /// @brief Construct from an exception
    template <class E> static expected<T> from_exception( const E &exception )
    {
        if ( typeid( exception ) != typeid( E ) )
            throw std::invalid_argument( "slicing detected" );
        return from_exception( std::make_exception_ptr( exception ) );
    }

    /// @brief Constructor from an exception pointer
    static expected<T> from_exception( std::exception_ptr p )
    {
        expected<T> result;
        result._got_value = false;
        new ( &result._exc ) std::exception_ptr( std::move( p ) );
        return result;
    }

    /// @brief Constructor from the current exception
    static expected<T> from_exception( void )
    {
        return from_exception( std::current_exception() );
    }

    /// @brief Do we have a value?
    bool valid( void ) const { return _got_value; }

    /// @brief Get the value (or throw the exception)
    T &get( void )
    {
        if ( !_got_value )
            std::rethrow_exception( _exc );
        return _value;
    }

    /// @brief Get the const value (or throw the exception)
    const T &get( void ) const
    {
        if ( !_got_value )
            std::rethrow_exception( _exc );
        return _value;
    }

    /// @brief Check if we have an exception of type E
    template <class E> bool has_exception( void ) const
    {
        try
        {
            if ( !_got_value )
                std::rethrow_exception( _exc );
        }
        catch ( const E &object )
        {
            return true;
        }
        catch ( ... )
        {}
        return false;
    }

    /// @brief Run a function and return an expected value
    template <class F> static expected from_code( F fun )
    {
        try
        {
            return expected( fun() );
        }
        catch ( ... )
        {
            return from_exception();
        }
    }

private:
    union
    {
        T                  _value;
        std::exception_ptr _exc;
    };

    bool _got_value;

    expected( void ) {}
};

////////////////////////////////////////

} // namespace base
