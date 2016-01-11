
#pragma once

#include <iostream>

namespace base
{

////////////////////////////////////////

template<typename Number>
class range
{
public:
	static_assert( std::is_integral<Number>::value, "Number must be integeral type" );

	constexpr range( Number f, Number l )
		: _first( std::min( f, l ) ), _last( std::max( f, l ) )
	{
	}

	constexpr Number first( void ) const
	{
		return _first;
	}

	constexpr Number last( void ) const
	{
		return _last;
	}

	void set( Number f, Number l )
	{
		_first = std::min( f, l );
		_last = std::max( f, l );
	}

	/// @brief Check if this and that overlap
	constexpr bool overlaps( const range &that ) const
	{
		return _first <= that._last && that._first <= _last;
	}

	/// @brief Check if that fits inside of this.
	constexpr bool covers( const range &that ) const
	{
		return _first <= that._first && _last >= that._last;
	}

	/// @brief Return the overlap between this and that.
	range overlap( const range &that ) const
	{
		Number a = std::max( _first, that._first );
		Number b = std::min( _last, that._last );

		precondition( a <= b, "ranges do not overlap" );
		return range( a, b );
	}

	/// @brief Return the range of this to the left of that.
	range left( const range &that ) const
	{
		Number a = _first;
		Number b = std::min( _last, that._first );
		if ( b < a )
			b = a;
		return range( a, b );
	}

	range right( const range &that ) const
	{
		Number a = _last;
		Number b = std::max( _first, that._last );
		if ( b < a )
			b = a;
		return range( a, b );
	}

	constexpr bool can_extend( Number x ) const
	{
		return ( x + 1 == _first ) || ( _last + 1 == x );
	}

	constexpr bool follows( const range &that ) const
	{
		return ( that._last + 1 == _first );
	}

	void extend( Number x )
	{
		_first = std::min( _first, x );
		_last = std::max( _last, x );
	}

	void extend( const range &that )
	{
		_first = std::min( _first, that._first );
		_last = std::max( _last, that._last );
	}

	constexpr bool operator<( const range &that ) const
	{
		return ( _first < that._first ) ? true : ( ( _first == that._first ) ? _last < that._last : false );
	}

	void clear( void )
	{
		_last = _first;
	}

private:
	Number _first;
	Number _last;
};

////////////////////////////////////////

template<typename N>
std::ostream &operator<<( std::ostream &out, const range<N> &r )
{
	out << r.first() << '-' << r.last();
	return out;
}

////////////////////////////////////////

}

