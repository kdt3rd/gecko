//
// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include <iostream>
#include <array>
#include <ratio>
#include <memory>
#include <base/pointer.h>
#include <base/contract.h>

namespace imgproc
{

////////////////////////////////////////

template<typename Type, size_t N>
class buffer
{
public:
	buffer( void )
	{
		for ( size_t i = 0; i < N; ++i )
			_offset[i] = _size[i] = _stride[i] = 0;
	}

	template<typename ...Args>
	buffer( Args ...args )
	{
		static_assert( N == sizeof...( args ), "invalid number of size" );

		for ( size_t i = 0; i < N; ++i )
			_offset[i] = _size[i] = _stride[i] = 0;

		set_size( args... );
	}

	template<typename ...Args>
	void set_size( Args ...args )
	{
		static_assert( N == sizeof...( args ), "invalid number of size" );
			
		std::array<int64_t,N> sizes{{ args... }};

		int64_t bytes = 1;
		for ( size_t i = 0; i < N; ++i )
		{
			_size[i] = sizes[i];
			bytes *= sizes[i];
		}

		_stride[0] = 1;
		for ( size_t i = 1; i < N; ++i )
			_stride[i] = _stride[i-1] * sizes[N-i-1];

		_data.reset( new Type[bytes], base::array_deleter<Type>() );
	}

	int64_t size( size_t dim ) const
	{
		return _size[dim];
	}

	template<typename ...Args>
	void set_offset( Args ...args )
	{
		static_assert( N == sizeof...( args ), "invalid number of size" );
		std::array<int64_t,N> off{{ args... }};

		for ( size_t i = 0; i < N; ++i )
			_offset[i] = off[i];
	}

	int64_t lower( size_t i ) const
	{
		return _offset[i];
	}	

	int64_t upper( size_t i ) const
	{
		return _offset[i] + _size[i];
	}	

	template<typename ...Args>
	int64_t offset_none( Args ...args ) const
	{
		std::array<int64_t,N> x{{ int64_t( args )... }};
		int64_t offset = 0;
		for ( size_t i = 0; i < N; ++i )
		{
			precondition( x[i] >= _offset[i] && x[i] - _offset[i] < _size[i], "out of range {0} ({1} - {2})", x[i], _offset[i], _offset[i] + _size[i] );
			offset += ( x[i] - _offset[i] ) * _stride[i];
		}
		return offset;
	}

	template<typename ...Args>
	int64_t offset_repeat( Args ...args ) const
	{
		std::array<int64_t,N> x{{ int64_t( args )... }};
		int64_t offset = 0;
		for ( size_t i = 0; i < N; ++i )
		{
			int64_t v = ( x[i] - _offset[i] ) % _size[i];
			offset += v * _stride[i];
		}
		return offset;
	}

	template<typename ...Args>
	int64_t offset_reflect( Args ...args ) const
	{
		std::array<int64_t,N> x{{ int64_t( args )... }};
		int64_t offset = 0;
		for ( size_t i = 0; i < N; ++i )
		{
			int64_t v = x[i] - _offset[i];
			bool done = false;
			while ( !done )
			{
				if ( v < 0 )
					v = -v;
				else if ( v >= _size[i] )
					v = v - 2 * ( v - _size[i] + 1 );
				else
					done = true;
			}

			offset += v * _stride[i];
		}
		return offset;
	}

	template<typename ...Args>
	int64_t offset_hold( Args ...args ) const
	{
		std::array<int64_t,N> x{{ int64_t( args )... }};
		int64_t offset = 0;
		for ( size_t i = 0; i < N; ++i )
		{
			int64_t v = ( x[i] - _offset[i] );
			if ( v < 0 )
				v = 0;
			if ( v >= _size[i] )
				v = _size[i] - 1;
			offset += v * _stride[i];
		}
		return offset;
	}

	template<typename ...Args>
	Type &operator()( Args ...args )
	{
		return *( _data.get() + offset_none( args... ) );
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return *( _data.get() + offset_none( args... ) );
	}

	template<typename ...Args>
	Type pixel_offset( int64_t off ) const
	{
		return *( _data.get() + off );
	}

	template<typename ...Args>
	Type pixel_none( Args ...args ) const
	{
		return pixel_offset( offset_none( args... ) );
	}

	template<typename ...Args>
	Type pixel_repeat( Args ...args ) const
	{
		return pixel_offset( offset_repeat( args... ) );
	}

	template<typename ...Args>
	Type pixel_reflect( Args ...args ) const
	{
		return pixel_offset( offset_reflect( args... ) );
	}

	template<typename ...Args>
	Type pixel_hold( Args ...args ) const
	{
		return pixel_offset( offset_hold( args... ) );
	}

	bool check( size_t i, int64_t x )
	{
		if ( x < _offset[i] && x - _offset[i] >= _size[i] )
			return false;
		return true;
	}

	int64_t calc_offset( size_t i, int64_t x )
	{
		return x * _stride[i];
	}

	void dump( void )
	{
		std::cout << "Sizes:";
		for ( size_t i = 0; i < N; ++i )
			std::cout << ' ' << _size[i];
		std::cout << std::endl;
		std::cout << "Strides:";
		for ( size_t i = 0; i < N; ++i )
			std::cout << ' ' << _stride[i];
		std::cout << std::endl;
	}

	const void *data( void ) const
	{
		return _data.get();
	}

protected:
	std::shared_ptr<Type> _data;
	int64_t _offset[N], _size[N], _stride[N];
};

////////////////////////////////////////

template<typename Type, size_t N>
class buffer_repeat
{
public:
	buffer_repeat( const buffer<Type,N> &img )
		: _img( &img )
	{
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return _img->pixel_repeat( args... );
	}

	int64_t upper( size_t i ) const
	{
		return _img->upper( i );
	}

	int64_t lower( size_t i ) const
	{
		return _img->lower( i );
	}

protected:
	const buffer<Type,N> *_img;
};

////////////////////////////////////////

template<typename Type, size_t N>
class buffer_reflect
{
public:
	buffer_reflect( const buffer<Type,N> &img )
		: _img( &img )
	{
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return _img->pixel_reflect( args... );
	}

	int64_t upper( size_t i ) const
	{
		return _img->upper( i );
	}

	int64_t lower( size_t i ) const
	{
		return _img->lower( i );
	}

protected:
	const buffer<Type,N> *_img;
};

////////////////////////////////////////

template<typename Type, size_t N>
class buffer_hold
{
public:
	buffer_hold( const buffer<Type,N> &img )
		: _img( &img )
	{
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return _img->pixel_hold( args... );
	}

	int64_t upper( size_t i ) const
	{
		return _img->upper( i );
	}

	int64_t lower( size_t i ) const
	{
		return _img->lower( i );
	}

protected:
	const buffer<Type,N> *_img;
};

////////////////////////////////////////

template<typename Type, size_t N, typename >
class buffer_constant0
{
public:
	buffer_constant0( const buffer<Type,N> &img )
		: _img( &img )
	{
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		std::array<int64_t,N> x{{ int64_t( args )... }};
		int64_t offset = 0;
		for ( size_t i = 0; i < N; ++i )
		{
			if ( !check( i, x[i] ) )
				return 0;
			offset += calc_offset( i, x[i] );
		}
		return _img->pixel_offset( offset );
	}

	int64_t upper( size_t i ) const
	{
		return _img->upper( i );
	}

	int64_t lower( size_t i ) const
	{
		return _img->lower( i );
	}

protected:
	const buffer<Type,N> *_img;
};

////////////////////////////////////////

/*
template<typename Buf>
int64_t _upper( const Buf &b, size_t n )
{
	return b.upper( n );
}

template<typename Buf>
int64_t _lower( const Buf &b, size_t n )
{
	return b.lower( n );
}

inline int64_t _upper( float x, size_t n )
{
//	precondition( n == 0, "dimension too large" );
	return static_cast<int64_t>( x );
}

inline int64_t _lower( float x, size_t n )
{
//	precondition( n == 0, "dimension too large" );
	return 0;
}
*/

////////////////////////////////////////

}

