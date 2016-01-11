
#pragma once
#include <iterator>

namespace base
{

////////////////////////////////////////

/// @brief Iterate over a range
template<typename It>
class iterator_range
{
public:
	iterator_range( It b, It e )
		: _begin( b ), _end( e )
	{
	}

	It begin( void ) const
	{
		return _begin;
	}

	It end( void ) const
	{
		return _end;
	}

	size_t size( void ) const
	{
		return _end - _begin;
	}

	typename It::value_type &operator[]( int i )
	{
		return (* (_begin + i ));
	}

	const typename It::value_type &operator[]( int i ) const
	{
		return (* (_begin + i ));
	}


private:
	It _begin;
	It _end;
};

////////////////////////////////////////

/// @brief Construct an iterator_range
template<typename It>
iterator_range<It> range( It b, It e )
{
	return iterator_range<It>( b, e );
}

////////////////////////////////////////

/// @brief Interate over column of two dimensional container
template<typename I, typename T, typename S>
class column_iterator : public std::iterator<std::random_access_iterator_tag, T>
{
	typedef std::iterator_traits<I> _traits;
public:
	typedef I iterator_type;

	typedef T value_type;
	typedef S size_type;
	typedef typename _traits::difference_type difference_type;
	typedef value_type &reference;
	typedef value_type *pointer;

	column_iterator( void ) : _iterator(), _n(0) {}
	explicit column_iterator( iterator_type b, size_type n )
		: _iterator( b ), _n( n )
	{
	}
	column_iterator( const column_iterator &x ) = default;
	// allow iterator type conversion based on
	// underlying
	template <typename OI>
	column_iterator( const column_iterator<OI, value_type, size_type> &x )
			: _iterator(x.base()), _n(x._n)
	{}

	iterator_type base( void ) const
	{
		return _iterator;
	}

	size_type offset( void ) const
	{
		return _n;
	}

	reference operator*( void ) const
	{
		return (*_iterator)[_n];
	}

	pointer operator->( void ) const
	{
		return &(operator*());
	}

	column_iterator &operator+=( difference_type i )
	{
		_iterator += i;
		return *this;
	}

	column_iterator &operator-=( difference_type i )
	{
		_iterator -= i;
		return *this;
	}

	column_iterator operator+( difference_type i ) const
	{
		return column_iterator( _iterator + i, _n );
	}

	column_iterator operator-( difference_type i ) const
	{
		return column_iterator( _iterator - i, _n );
	}

	difference_type operator-( const column_iterator &i ) const
	{
		return _iterator - i._iterator;
	}

	column_iterator &operator++( void )
	{
		_iterator++;
		return *this;
	}
	column_iterator operator++( int )
	{
		column_iterator t = *this;
		++_iterator;
		return t;
	}

	column_iterator &operator--( void )
	{
		_iterator--;
		return *this;
	}
	column_iterator operator--( int )
	{
		column_iterator t = *this;
		--_iterator;
		return t;
	}

	reference operator[]( difference_type i ) const
	{
		return (* (_iterator + i ))[_n];
	}

private:
	I _iterator;
	size_type _n;
};

template <typename I, typename T, typename S>
inline bool operator==( const column_iterator<I, T, S> &a, const column_iterator<I, T, S> &b )
{
	return ( a.base() == b.base() && a.offset() == b.offset() );
}

template <typename I, typename T, typename S>
inline bool operator!=( const column_iterator<I, T, S> &a, const column_iterator<I, T, S> &b )
{
	return !( a == b );
}


////////////////////////////////////////

/// @brief Construct a column iterator
template<typename C>
iterator_range<column_iterator<typename C::iterator, typename C::value_type::value_type, typename C::size_type>> column( C &c, typename C::size_type n )
{
	typedef column_iterator<typename C::iterator, typename C::value_type::value_type, typename C::size_type> iter_type;
	return iterator_range<iter_type>( iter_type( c.begin(), n ),
									  iter_type( c.end(), n ) );
}

////////////////////////////////////////

/// @brief Construct a row iterator
template<typename C>
iterator_range<typename C::iterator> rows( C &c )
{
	return range( c.begin(), c.end() );
}

////////////////////////////////////////

/// @brief Construct a list of column iterators
template<typename C>
std::vector<iterator_range<column_iterator<typename C::iterator, typename C::value_type::value_type, typename C::size_type>>> columns( C &c )
{
	typedef column_iterator<typename C::iterator, typename C::value_type::value_type, typename C::size_type> iter_type;
	std::vector<iterator_range<iter_type>> ranges;
	if ( !c.empty() )
	{
		auto &row = c[0];
		for ( typename C::size_type i = 0; i < row.size(); ++i )
		{
			ranges.emplace_back(
				iter_type( c.begin(), i ),
				iter_type( c.end(), i ) );
		}
	}

	return ranges;
}

////////////////////////////////////////

}

