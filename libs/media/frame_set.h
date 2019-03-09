// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

////////////////////////////////////////

namespace media
{

class frame_set_citer
{
public:
	// required defs for iterator_traits
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef int64_t value_type;
	typedef int64_t difference_type;
	typedef const int64_t &reference;
	typedef const int64_t *pointer;
	
	typedef std::vector<std::pair<int64_t, int64_t>> fset_list;
	typedef fset_list::const_iterator accessor;

	inline frame_set_citer( void ) = default;
	explicit inline frame_set_citer( const accessor &a, int64_t point ) : _a( a ), _cur( point ) {}

	reference operator*( void ) const { return _cur; }
	pointer operator->( void ) const { return &_cur; }

	inline frame_set_citer &operator++( void )
	{
		increment();
		return *this;
	}

	inline frame_set_citer operator++( int )
	{
		frame_set_citer ret( *this );
		increment();
		return ret;
	}

	inline frame_set_citer &operator--( void )
	{
		decrement();
		return *this;
	}

	inline frame_set_citer operator--( int )
	{
		frame_set_citer ret( *this );
		decrement();
		return ret;
	}

	inline bool operator==( const frame_set_citer &o ) const
	{
		return _a == o._a && _cur == o._cur;
	}
	inline bool operator!=( const frame_set_citer &o ) const
	{
		return _a != o._a || _cur != o._cur;
	}

private:
	inline void increment( void )
	{
		++_cur;
		if ( _cur > _a->second )
		{
			++_a;
			_cur = _a->first;
		}
	}

	inline void decrement( void )
	{
		--_cur;
		if ( _cur < _a->first )
		{
			--_a;
			_cur = _a->second;
		}
	}

	accessor _a;
	int64_t _cur = -1;
};

///
/// @brief Class frame_set provides a wrapper around a potentially disjoint set of frames.
///
/// This allows  
///
class frame_set
{
public:
	typedef frame_set_citer const_iterator;

	frame_set( void ) = default;
	~frame_set( void );
	frame_set( const frame_set & ) = default;
	frame_set( frame_set && ) = default;
	frame_set &operator=( const frame_set & ) = default;
	frame_set &operator=( frame_set && ) = default;

	void add( int64_t f );

	bool contains( int64_t f ) const;

	inline bool empty( void ) const;
	inline bool has_gaps( void ) const;

	inline size_t ranges( void ) const;
	inline std::pair<int64_t, int64_t> range( size_t i ) const;

	inline size_t count( void ) const;

	inline int64_t first( void ) const;
	inline int64_t last( void ) const;

	inline const_iterator begin( void ) const;
	inline const_iterator end( void ) const;

private:
	size_t _count = 0;
	std::vector<std::pair<int64_t, int64_t>> _ranges;
};

////////////////////////////////////////

inline bool frame_set::empty( void ) const
{
	return _ranges.empty();
}

////////////////////////////////////////

inline bool frame_set::has_gaps( void ) const
{
	return ranges() > 1;
}

////////////////////////////////////////

inline size_t frame_set::ranges( void ) const
{
	return _ranges.size();
}

////////////////////////////////////////

inline std::pair<int64_t, int64_t> frame_set::range( size_t i ) const
{
	return _ranges[i];
}

////////////////////////////////////////

inline size_t frame_set::count( void ) const
{
	return _count;
}

////////////////////////////////////////

inline int64_t frame_set::first( void ) const
{
	return _ranges.empty() ? -1 : _ranges.front().first;
}

////////////////////////////////////////

inline int64_t frame_set::last( void ) const
{
	return _ranges.empty() ? -1 : _ranges.back().second;
}

////////////////////////////////////////

inline frame_set::const_iterator frame_set::begin( void ) const
{
	return const_iterator( _ranges.begin(), first() );
}

////////////////////////////////////////

inline frame_set::const_iterator frame_set::end( void ) const
{
	return const_iterator( _ranges.end(), last() + 1 );
}

std::ostream &operator<<( std::ostream &os, const frame_set &fs );

} // namespace media



