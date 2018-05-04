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
#include <base/endian.h>

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
	image_buffer( const std::shared_ptr<T> &data, int64_t x1, int64_t y1, int64_t x2, int64_t y2 )
		: _data( data ),
		  _offset( 0 ),
		  _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 ),
		  _width( x2 - x1 + 1 ), _height( y2 - y1 + 1 ),
		  _xstride( static_cast<int64_t>(sizeof(T)*8) ),
		  _ystride( static_cast<int64_t>(sizeof(T)*8)*( x2 - x1 + 1 ) ),
		  _bits( static_cast<int16_t>(sizeof(T) * 8) ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t xstride, int64_t ystride, int64_t offset = 0, base::endianness e = base::endianness::NATIVE )
		: _data( data ),
		  _offset( offset ),
		  _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 ),
		  _width( x2 - x1 + 1 ), _height( y2 - y1 + 1 ),
		  _xstride( xstride ), _ystride( ystride ),
		  _endian( e ),
		  _bits( static_cast<int16_t>( sizeof(T) * 8 ) ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
		precondition( offset % 8 == 0, "offset should be multiple of 8 bits" );
	}

	image_buffer( const image_buffer &interleaved, int64_t offset )
		: _data( interleaved._data ),
		  _offset( offset ),
		  _x1( interleaved._x1 ), _y1( interleaved._y1 ),
		  _x2( interleaved._x2 ), _y2( interleaved._y2 ),
		  _width( interleaved._width ), _height( interleaved._height ),
		  _xstride( interleaved._xstride ), _ystride( interleaved._ystride ),
		  _endian( interleaved._endian ),
		  _bits( interleaved._bits ),
		  _ysubsample( interleaved._ysubsample ), 
		  _ysubsample_shift( interleaved._ysubsample_shift ), 
		  _xsubsample( interleaved._xsubsample ), 
		  _xsubsample_shift( interleaved._xsubsample_shift ), 
		  _floating( interleaved._floating ),
		  _unsigned( interleaved._unsigned )
	{
		precondition( offset % 8 == 0, "offset should be multiple of 8 bits" );
	}
		
	image_buffer( const std::shared_ptr<void> &data, int16_t bits, int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t xstride, int64_t ystride, int64_t offset, base::endianness e, bool isfloat, bool isunsign )
		: _data( data ),
		  _offset( offset ),
		  _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 ),
		  _width( x2 - x1 + 1 ), _height( y2 - y1 + 1 ),
		  _xstride( xstride ), _ystride( ystride ),
		  _endian( e ),
		  _bits( bits ),
		  _floating( isfloat ),
		  _unsigned( isunsign )
	{
		precondition( offset % 8 == 0, "offset should be multiple of 8 bits" );
		precondition( ( isfloat && ! isunsign ) || ( ! isfloat ), "float must be signed" );
	}

	image_buffer( const image_buffer & ) = default;
	image_buffer( image_buffer && ) = default;
	image_buffer &operator=( const image_buffer & ) = default;
	image_buffer &operator=( image_buffer && ) = default;
	~image_buffer( void ) = default;

	inline int64_t x1( void ) const { return _x1; }
	inline int64_t y1( void ) const { return _y1; }
	inline int64_t x2( void ) const { return _x2; }
	inline int64_t y2( void ) const { return _y2; }

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
	inline int64_t offset( void ) const { return _offset; }

	inline void set_endianness( base::endianness e ) { _endian = e; }
	inline base::endianness endianness( void ) const { return _endian; }

	const std::shared_ptr<void> &raw( void ) const { return _data; }

	void *data( void )
	{
		return static_cast<char *>( _data.get() ) + _offset / 8;
	}

	const void *data( void ) const
	{
		return static_cast<const char *>( _data.get() ) + _offset / 8;
	}

	void *row( int64_t y )
	{
		uint8_t *data = static_cast<uint8_t *>( _data.get() );
		data += ( _offset + ( ( y - _y1 ) >> _ysubsample_shift ) * _ystride ) / 8;
		return data;
	}

	const void *row( int64_t y ) const
	{
		const uint8_t *data = static_cast<const uint8_t *>( _data.get() );
		data += ( _offset + ( ( y - _y1 ) >> _ysubsample_shift ) * _ystride ) / 8;
		return data;
	}

	template<typename T>
	static image_buffer simple_buffer( int64_t x1, int64_t y1, int64_t x2, int64_t y2 )
	{
		int64_t w = ( x2 - x1 + 1 );
		int64_t h = ( y2 - y1 + 1 );

		// TBD: make each line sse (avx2) 256-bit (32-byte) aligned? we would have to
		// adjust opengl texturing and a few other places if we do that, and wouldn't be useable
		// on formats that just read into a flat memory buffer...
		size_t linebytes = w*sizeof(T);
		int64_t bits = static_cast<int16_t>( sizeof(T) * 8 );
		int64_t xstride = static_cast<int64_t>( sizeof(T) );
		int64_t ystride = static_cast<int64_t>( linebytes );
		constexpr bool isf = std::is_floating_point<T>::value;
		constexpr bool isu = std::is_unsigned<T>::value;

		std::shared_ptr<void> vdata = std::shared_ptr<uint8_t>( new uint8_t[ystride * h], base::array_deleter<uint8_t>() );
		return image_buffer( vdata, bits, x1, y1, x2, y2, xstride * 8, ystride * 8, 0,
							 base::endianness::NATIVE, isf, isu );
	}

	template<typename T>
	static image_buffer simple_interleaved( int64_t w, int64_t h, int chans )
	{
		// TBD: make each line sse (avx2) 256-bit (32-byte) aligned? we would have to
		// adjust opengl texturing and a few other places if we do that, and wouldn't be useable
		// on formats that just read into a flat memory buffer...
		int16_t bits = static_cast<int16_t>( sizeof(T) * 8 );
		size_t linebytes = static_cast<size_t>( w * chans ) * sizeof(T);
		int64_t xstride = static_cast<int64_t>( sizeof(T) ) * chans;
		int64_t ystride = static_cast<int64_t>( linebytes );
		constexpr bool isf = std::is_floating_point<T>::value;
		constexpr bool isu = std::is_unsigned<T>::value;

		std::shared_ptr<void> vdata = std::shared_ptr<uint8_t>( new uint8_t[ystride * h], base::array_deleter<uint8_t>() );
		return image_buffer( vdata, bits, 0, 0, w - 1, h - 1, xstride * 8, ystride * 8, 0,
							 base::endianness::NATIVE, isf, isu );
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
	int64_t _offset = 0;
	int64_t _x1 = 0;
	int64_t _y1 = 0;
	int64_t _x2 = 0;
	int64_t _y2 = 0;
	int64_t _width = 0;
	int64_t _height = 0;
	int64_t _xstride = 0;
	int64_t _ystride = 0;
	base::endianness _endian = base::endianness::NATIVE;
	int16_t _bits = 8;
	int8_t _ysubsample = 1;
	int8_t _ysubsample_shift = 0;
	int8_t _xsubsample = 1;
	int8_t _xsubsample_shift = 0;
	bool _floating = false;
	bool _unsigned = true;
};

////////////////////////////////////////

}

