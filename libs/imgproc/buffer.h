
#pragma once

#include <iostream>
#include <array>
#include <memory>
#include <base/pointer.h>
#include <base/contract.h>

namespace imgproc
{

////////////////////////////////////////

template<typename Type, size_t N>
class boundary_none
{
public:
	template<typename ...Args>
	int64_t offset( Args ...args ) const
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
	Type &operator()( Args ...args )
	{
		return *(_data.get() + offset( args... ));
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return *(_data.get() + offset( args... ));
	}

protected:
	std::shared_ptr<Type> _data;
	int64_t _offset[N], _size[N], _stride[N];
};

////////////////////////////////////////

template<typename Type, size_t N>
class boundary_repeat
{
public:
	template<typename ...Args>
	int64_t offset( Args ...args ) const
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
	Type &operator()( Args ...args )
	{
		return *(_data.get() + offset( args... ));
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return *(_data.get() + offset( args... ));
	}

protected:
	std::shared_ptr<Type> _data;
	int64_t _offset[N], _size[N], _stride[N];
};

////////////////////////////////////////

template<typename Type, size_t N>
class boundary_reflect
{
public:
	template<typename ...Args>
	int64_t offset( Args ...args ) const
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
	Type &operator()( Args ...args )
	{
		return *(_data.get() + offset( args... ));
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return *(_data.get() + offset( args... ));
	}

protected:
	std::shared_ptr<Type> _data;
	int64_t _offset[N], _size[N], _stride[N];
};

////////////////////////////////////////

template<typename Type, size_t N>
class boundary_hold
{
public:
	template<typename ...Args>
	int64_t offset( Args ...args ) const
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
		return *(_data.get() + offset( args... ));
	}

	template<typename ...Args>
	Type operator()( Args ...args ) const
	{
		return *(_data.get() + offset( args... ));
	}

protected:
	std::shared_ptr<Type> _data;
	int64_t _offset[N], _size[N], _stride[N];
};

////////////////////////////////////////

template<typename Type, size_t N, template<typename,size_t> class Boundary=boundary_reflect>
class buffer : public Boundary<Type,N>
{
public:
	buffer( void )
	{
		for ( size_t i = 0; i < N; ++i )
			this->_offset[i] = this->_size[i] = this->_stride[i] = 0;
	}

	template<typename ...Args>
	buffer( Args ...args )
	{
		static_assert( N == sizeof...( args ), "invalid number of size" );

		for ( size_t i = 0; i < N; ++i )
			this->_offset[i] = this->_size[i] = this->_stride[i] = 0;

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
			this->_size[i] = sizes[i];
			bytes *= sizes[i];
		}

		this->_stride[0] = 1;
		for ( size_t i = 1; i < N; ++i )
			this->_stride[i] = this->_stride[i-1] * sizes[N-i-1];

		this->_data.reset( new Type[bytes], base::array_deleter<Type>() );
	}

	int64_t size( size_t dim ) const
	{
		return this->_size[dim];
	}

	template<typename ...Args>
	void set_offset( Args ...args )
	{
		static_assert( N == sizeof...( args ), "invalid number of size" );
		std::array<int64_t,N> off{{ args... }};

		for ( size_t i = 0; i < N; ++i )
			this->_offset[i] = off[i];
	}

	int64_t lower( size_t i ) const
	{
		return this->_offset[i];
	}	

	int64_t upper( size_t i ) const
	{
		return this->_offset[i] + this->_size[i];
	}	

	void dump( void )
	{
		std::cout << "Sizes:";
		for ( size_t i = 0; i < N; ++i )
			std::cout << ' ' << this->_size[i];
		std::cout << std::endl;
		std::cout << "Strides:";
		for ( size_t i = 0; i < N; ++i )
			std::cout << ' ' << this->_stride[i];
		std::cout << std::endl;
	}

	const void *data( void ) const
	{
		return this->_data.get();
	}
};

////////////////////////////////////////

template<typename Type, size_t N>
int64_t _upper( const buffer<Type,N> &b, size_t n )
{
	precondition( n < N, "dimension too large" );
	return b.upper( n );
}

template<typename Type, size_t N>
int64_t _lower( const buffer<Type,N> &b, size_t n )
{
	precondition( n < N, "dimension too large" );
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

////////////////////////////////////////

}

