
#pragma once

#include "record.h"

namespace model
{

////////////////////////////////////////

template<typename T>
class field
{
public:
	field( void ) = delete;

	field( record *r )
		: _rec( r )
	{
		assert( r != nullptr );
	}

	template<typename ...Args>
	field( record *r, Args &&...args )
		: _rec( r ), _value( std::forward<Args>( args )... )
	{
		assert( r != nullptr );
	}

	field &operator=( const T &v )
	{
		_value = v;
		if ( _rec )
			_rec->changed();
		return *this;
	}

	field &operator=( T &&v )
	{
		_value = std::forward<T>( v );
		if ( _rec )
			_rec->changed();
		return *this;
	}

	const T &value( void ) const
	{
		return _value;
	}

	const T *operator->( void ) const
	{
		return &_value;
	}

private:
	record *_rec = nullptr;
	T _value;
};

////////////////////////////////////////

template <typename T> using shared_field = field<std::shared_ptr<T>>;

////////////////////////////////////////

}

