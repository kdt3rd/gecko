
#pragma once

#include <memory>
#include "record.h"
#include "field.h"
#include <base/meta.h>

namespace model
{

namespace detail
{
	template<typename T>
	struct placeholder : public record
	{
		template<typename ...Args>
		placeholder( Args &&...args )
			: value( this, std::forward<Args>( args )... )
		{
		}

		field<T> value = field<T>( this );
	};
}

////////////////////////////////////////

template<typename T>
class datum
{
public:
	datum( void )
		: _record( std::make_shared<detail::placeholder<T>>() ), _field( reinterpret_cast<detail::placeholder<T>*>(_record.get())->value )
	{
	}

	datum( const datum &d ) = delete;

	datum( datum &&d )
		: _record( d._record ), _field( d._field ), _connection( d._connection )
	{
		d._connection = 0;
	}

	template<typename ...Args>
	datum( Args &&...args )
		: _record( std::make_shared<detail::placeholder<T>>( std::forward<Args>( args )... ) ), _field( reinterpret_cast<detail::placeholder<T>*>(_record.get())->value )
	{
	}

	datum( const std::shared_ptr<record> &r, field<T> &f )
		: _record( r ), _field( f )
	{
	}

	~datum( void )
	{
		if ( _connection != 0 )
			_record->changes -= _connection;
	}

	datum &operator=( const datum &d ) = delete;

	template<typename U, typename X = disable_if_same_or_derived<datum,U>>
	datum &operator=( U &&v )
	{
		_field = std::forward<U>( v );
		return *this;
	}

	const T &value( void ) const
	{
		return _field.value();
	}

	void operator+=( const std::function<void(void)> &cb )
	{
		if ( _connection != 0 )
			_record->changes -= _connection;
		_connection = _record->changes += cb;
	}

private:
	std::shared_ptr<record> _record;
	field<T> &_field;
	base::connection _connection = 0;
};

////////////////////////////////////////

template <typename T> using shared_datum = datum<std::shared_ptr<T>>;

////////////////////////////////////////

template<typename T>
datum<T> make_datum( const T &v )
{
	return datum<T>( v );
}

////////////////////////////////////////

template<typename T>
datum<T> make_datum( const std::shared_ptr<record> &r, field<T> &f )
{
	precondition( f.belongs_to( r ), "field should belong to record" );
	return datum<T>( r, f );
}

////////////////////////////////////////

}

