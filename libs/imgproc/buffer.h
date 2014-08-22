
#pragma once

#include <iostream>
#include <array>
#include <memory>

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

		_data.reset( new Type[bytes] );
	}

	int64_t size( size_t dim )
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


	int64_t offset( int64_t x[N] ) const
	{
		int64_t offset = 0;
		for ( size_t i = 0; i < N; ++i )
			offset += ( x[i] - _offset[i] ) * _stride[i];
		return offset;
	}

	template<typename ...Args>
	Type &operator()( Args ...args )
	{
		std::array<int64_t,N> x{{ int64_t( args )... }};
		return *(_data.get() + offset( x.data() ));
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		std::array<int64_t,N> x{{ args... }};
		return *(_data.get() + offset( x.data() ));
	}

	int64_t lower( size_t i ) const
	{
		return _offset[i];
	}	

	int64_t upper( size_t i ) const
	{
		return _offset[i] + _size[i];
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

private:
	std::shared_ptr<Type> _data;
	int64_t _offset[N], _size[N], _stride[N];
};

////////////////////////////////////////

}

