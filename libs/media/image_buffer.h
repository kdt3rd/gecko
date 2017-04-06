//
// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>
#include <base/half.h>
#include <base/pointer.h>
#include <base/contract.h>

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
	template<typename T> struct type {};

	image_buffer( void )
	{
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, int64_t w, int64_t h )
		: _data( data ),
		  _bits( static_cast<int64_t>(sizeof(T) * 8) ),
		  _offset( 0 ),
		  _width( w ), _height( h ),
		  _xstride( static_cast<int64_t>(sizeof(T)*8) ),
		  _ystride( static_cast<int64_t>(sizeof(T)*8)*w ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, int64_t w, int64_t h, int64_t xstride, int64_t ystride )
		: _data( data ),
		  _bits( sizeof(T) * 8 ),
		  _offset( 0 ),
		  _width( w ), _height( h ),
		  _xstride( xstride ), _ystride( ystride ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
	}

	inline int64_t width( void ) const
	{
		return _width;
	}

	inline int64_t height( void ) const
	{
		return _height;
	}

	inline int64_t bits( void ) const
	{
		return _bits;
	}

	inline bool is_floating( void ) const
	{
		return _floating;
	}

	inline int64_t xstride_bytes( void ) const
	{
		return _xstride / 8;
	}

	inline int64_t ystride_bytes( void ) const
	{
		return _ystride / 8;
	}

	void get_scanline( int64_t y, float *line, int64_t stride = 1 ) const;

	void set_scanline( int64_t y, const float *line, int64_t stride = 1 );

	void set_offset( int64_t offset_bits )
	{
		precondition( offset_bits % 8 == 0, "offset should be multiple of 8 bits" );
		_offset = offset_bits;
	}

	void *data( void )
	{
		return static_cast<char *>( _data.get() ) + _offset / 8;
	}

	const void *data( void ) const
	{
		return static_cast<const char *>( _data.get() ) + _offset / 8;
	}

	template<typename T>
	static image_buffer simple_buffer( int64_t w, int64_t h )
	{
		auto data = std::shared_ptr<T>( new T[static_cast<size_t>(w*h)], base::array_deleter<T>() );
		return image_buffer( data, w, h );
	}

private:
	void get_scanline_u8( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_u16( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_f16( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_f32( int64_t y, float *line, int64_t stride ) const;
	void get_scanline_f64( int64_t y, float *line, int64_t stride ) const;

	void set_scanline_u8( int64_t y, const float *line, int64_t stride );
	void set_scanline_u16( int64_t y, const float *line, int64_t stride );
	void set_scanline_f16( int64_t y, const float *line, int64_t stride );
	void set_scanline_f32( int64_t y, const float *line, int64_t stride );
	void set_scanline_f64( int64_t y, const float *line, int64_t stride );

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

