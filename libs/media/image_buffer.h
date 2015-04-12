
#pragma once

#include <cstdint>
#include <base/half.h>
#include <base/pointer.h>

namespace media
{

////////////////////////////////////////

/// @brief 2D memory buffer containing a single plane of an image.
///
/// When acessing pixel (x,y), the memory is at:
///   bit_location = offset + ( x / xsubsample ) * xstride + ( y / ysubsample ) * ystride
///   The data is then at base_ptr[bit_location .. bit_location + bits]
///
class image_buffer
{
public:
	template<typename T>
	image_buffer( int64_t w, int64_t h )
		: _data( static_cast<void*>( new T[w*h] ), base::array_deleter<T>() ),
		  _bits( sizeof(T) * 8 ),
		  _offset( 0 ),
		  _width( w ), _height( h ),
		  _xstride( sizeof(T)*8 ), _ystride( sizeof(T)*8*w ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
		static_assert( sizeof(T) % 8 == 0, "data type should be multiple of 8 bits" );
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, int64_t w, int64_t h )
		: _data( data ), _width( w ), _height( h ),
		  _bits( sizeof(T) * 8 ),
		  _offset( 0 ),
		  _width( w ), _height( h ),
		  _xstride( sizeof(T)*8 ), _ystride( sizeof(T)*8*w ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
		static_assert( sizeof(T) % 8 == 0, "data type should be multiple of 8 bits" );
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, int64_t w, int64_t h, int64_t xstride, int64_t ystride )
		: _data( data ), _width( w ), _height( h ),
		  _bits( sizeof(T) * 8 ),
		  _offset( 0 ),
		  _width( w ), _height( h ),
		  _xstride( xstride ), _ystride( ystride ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
		static_assert( sizeof(T) % 8 == 0, "data type should be multiple of 8 bits" );
	}

	void get_scanline( int64_t y, float *line, int64_t stride = 1 ) const;

private:
	void get_scanline_u8( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_u16( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_f16( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_f32( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_f64( int64_t y, float *line, int64_t stride ) const;

	std::shared_ptr<void> _data;
	int64_t _bits = 8;
	int64_t _offset = 0;
	int64_t _width = 0;
	int64_t _height = 0;
	int64_t _xstride = 0;
	int64_t _ystride = 0;
	int64_t _ysubsample = 1;
	int64_t _xsubsample = 1;
	bool _floating = false;
	bool _unsigned = true;
};

////////////////////////////////////////

}

