
#pragma once

#include <type_traits>
#include <utility>
#include <typeinfo>
#include <string>
#include <cassert>
#include <memory>

////////////////////////////////////////

class any
{
public:
	template<class T> using decay = typename std::decay<T>::type;

	template<typename U>
	any( U &&value )
		: _ptr( new derived<decay<U>>( std::forward<U>( value ) ) )
	{
	}

	any( void )
	{
	}

	any( any &that)
		: _ptr( that.clone() )
	{
	}

	any( any &&that)
		: _ptr( std::move( that._ptr ) )
	{
	}

	any( const any &that)
		: _ptr( that.clone() )
	{
	}

	any( const any &&that )
		: _ptr( that.clone() )
	{
	}

	bool is_null() const
	{
		return !bool( _ptr );
	}

	bool not_null() const
	{
		return bool( _ptr );
	}

	template<class U>
	bool is( void ) const
	{
	    typedef decay<U> T;
	    auto d = dynamic_cast<derived<T>*>( _ptr.get() );
	    return bool( d );
	}

	template<class U>
	decay<U> &as( void )
	{
	    typedef decay<U> T;
	    auto d = dynamic_cast<derived<T>*>( _ptr.get() );
	    if ( !d )
		{
			std::cout << "Ooops: " << typeid( _ptr.get() ).name() << std::endl;
	        throw std::bad_cast();
		}
	    return d->_value;
	}

	template<class U>
	const decay<U> &as( void ) const
	{
	    typedef decay<U> T;
	    auto d = dynamic_cast<derived<T>*>( _ptr.get() );
	    if ( !d )
	        throw std::bad_cast();
	    return d->_value;
	}

	template<class U>
	operator U() const
	{
	    return as<decay<U>>();
	}

	any &operator=( const any &a )
	{
	    if ( _ptr == a._ptr )
	        return *this;
	    _ptr = a.clone();
	    return *this;
	}

	any &operator=( any &&a )
	{
		using std::swap;
	    if ( _ptr == a._ptr )
	        return *this;
	    swap( _ptr, a._ptr );
	    return *this;
	}

private:
	class base
	{
	public:
	    virtual ~base( void ) {}
	    virtual std::unique_ptr<base> clone( void ) const = 0;
	};

	template<typename T>
	class derived : base
	{
	public:
	    template<typename U>
		derived( U &&value )
			: _value( std::forward<U>( value ) )
		{
		}

		std::unique_ptr<base> clone( void ) const
		{
			return std::unique_ptr<base>( new derived<T>( _value ) );
		}

	private:
		friend class any;
	    T _value;
	};

	std::unique_ptr<base> clone( void ) const
	{
		return _ptr ? _ptr->clone() : nullptr;
	}

	std::unique_ptr<base> _ptr;
};

////////////////////////////////////////

