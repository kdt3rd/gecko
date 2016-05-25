
#pragma once

#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>
#include <memory>
#include "contract.h"

namespace base
{

class bad_any_cast : public std::runtime_error
{
public:
	virtual ~bad_any_cast( void );
	bad_any_cast( const bad_any_cast & ) = default;
	bad_any_cast( bad_any_cast && ) = default;
	bad_any_cast &operator=( const bad_any_cast & ) = default;
	bad_any_cast &operator=( bad_any_cast && ) = default;
	using std::runtime_error::runtime_error;
};
#define throw_bad_any_cast( ... ) \
	throw_location( base::bad_any_cast( base::format( __VA_ARGS__ ) ) )

////////////////////////////////////////

/// @brief Holds a value of any type.
///
/// This is similar to a void pointer (void *), but is type-safe.
class any
{
public:
	/// @brief Decay type alias
	template<class T> using decay = typename std::decay<T>::type;

	/// @brief Default constructor
	/// It contains no value until one is assigned.
	any( void )
	{
	}

	/// @brief Constuct from the given r-value
	template<typename U>
	any( U &&value )
		: _ptr( new derived<decay<U>>( std::forward<U>( value ) ) )
	{
	}

	/// @brief Copy constructor
	any( const any &that)
		: _ptr( that.clone() )
	{
	}

	/// @brief Move constructor
	any( any &&that)
		: _ptr( std::move( that._ptr ) )
	{
	}

	/// @brief Is the any value not set?
	bool is_null() const
	{
		return !bool( _ptr );
	}

	/// @brief Is the any value set?
	bool not_null() const
	{
		return bool( _ptr );
	}

	/// @brief any value set per c++14 function name
	inline bool empty( void ) const
	{
		return ! bool( _ptr );
	}

	/// @brief Is the any value of type U?
	template<class U>
	bool is_type( void ) const
	{
	    typedef decay<U> T;
	    auto d = dynamic_cast<derived<T>*>( _ptr.get() );
	    return bool( d );
	}

	/// @brief Access the any value as type U
	template<class U>
	decay<U> &as( void )
	{
	    typedef decay<U> T;
	    auto d = dynamic_cast<derived<T>*>( _ptr.get() );
	    if ( !d )
	        throw_bad_any_cast( "bad any_cast: request type {0} but ptr is type {1}", typeid(U).name(), typeid(_ptr).name() );

	    return d->_value;
	}

	/// @brief Access the any value as type const U
	template<class U>
	const decay<U> &as( void ) const
	{
	    typedef decay<U> T;
	    auto d = dynamic_cast<derived<T>*>( _ptr.get() );
	    if ( !d )
	        throw_bad_any_cast( "bad any_cast: request type {0} but ptr is type {1}", typeid(U).name(), typeid(_ptr).name() );

		return d->_value;
	}

	/// @brief Cast operator to type U
	template<class U>
	operator U() const
	{
	    return as<decay<U>>();
	}

	/// @brief Assignment operator
	any &operator=( const any &a )
	{
	    if ( _ptr == a._ptr )
	        return *this;
	    _ptr = a.clone();
	    return *this;
	}

	/// @brief Assignment move operator
	any &operator=( any &&a )
	{
		using std::swap;
	    if ( _ptr == a._ptr )
	        return *this;
	    swap( _ptr, a._ptr );
	    return *this;
	}

	template <class S>
	inline void binary_stream( S &s ) const
	{
		if ( _ptr )
		{
			size_t bytes = 0;
			const void *rawPtr = _ptr->binary_stream_ptr( bytes );
			s.add( rawPtr, bytes );
		}
		else
			s << "<any: null>";
	}
private:
	class any_base
	{
	public:
	    virtual ~any_base( void );
	    virtual std::unique_ptr<any_base> clone( void ) const = 0;
		virtual const void *binary_stream_ptr( size_t &s ) const = 0;
	};

	template<typename T>
	class derived : public any_base
	{
	public:
	    template<typename U>
		derived( U &&value )
			: _value( std::forward<U>( value ) )
		{
		}

		std::unique_ptr<any_base> clone( void ) const
		{
			return std::unique_ptr<any_base>( new derived<T>( _value ) );
		}

		virtual const void *binary_stream_ptr( size_t &s ) const
		{
			s = sizeof(_value);
			return &_value;
		}

	private:
		friend class any;
	    T _value;
	};

	std::unique_ptr<any_base> clone( void ) const
	{
		return _ptr ? _ptr->clone() : nullptr;
	}

	std::unique_ptr<any_base> _ptr;
};

template <class T>
T any_cast( const any &a )
{
	return a.as<T>();
}

template <class T>
T any_cast( any &a )
{
	return a.as<T>();
}

////////////////////////////////////////

}

