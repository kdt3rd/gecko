// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <base/half.h>
#include <base/rect.h>
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
	using rect = base::rect<int64_t>;
	template<typename T> struct type {};

	image_buffer( void )
	{
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, const rect &r )
		: _data( data ),
		  _offset( 0 ),
		  _region( r ),
		  _xstride( static_cast<int64_t>(sizeof(T)*8) ),
		  _ystride( static_cast<int64_t>(sizeof(T)*8) * r.width() ),
		  _bits( static_cast<int16_t>(sizeof(T) * 8) ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, int64_t x1, int64_t y1, int64_t x2, int64_t y2 )
		: _data( data ),
		  _offset( 0 ),
		  _region( rect::from_points( x1, y1, x2, y2 ) ),
		  _xstride( static_cast<int64_t>(sizeof(T)*8) ),
		  _ystride( static_cast<int64_t>(sizeof(T)*8)*( x2 - x1 + 1 ) ),
		  _bits( static_cast<int16_t>(sizeof(T) * 8) ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, const rect &r, int64_t xstride, int64_t ystride, int64_t offset = 0, base::endianness e = base::endianness::NATIVE )
		: _data( data ),
		  _offset( offset ),
		  _region( r ),
		  _xstride( xstride ), _ystride( ystride ),
		  _endian( e ),
		  _bits( static_cast<int16_t>( sizeof(T) * 8 ) ),
		  _floating( std::is_floating_point<T>::value ),
		  _unsigned( std::is_unsigned<T>::value )
	{
		precondition( offset % 8 == 0, "offset should be multiple of 8 bits" );
	}

	template<typename T>
	image_buffer( const std::shared_ptr<T> &data, int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t xstride, int64_t ystride, int64_t offset = 0, base::endianness e = base::endianness::NATIVE )
		: _data( data ),
		  _offset( offset ),
		  _region( rect::from_points( x1, y1, x2, y2 ) ),
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
		  _region( interleaved._region ),
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

	image_buffer( const std::shared_ptr<void> &data, int16_t bits, const rect &r, int64_t xstride, int64_t ystride, int64_t offset, base::endianness e, bool isfloat, bool isunsign )
		: _data( data ),
		  _offset( offset ),
		  _region( r ),
		  _xstride( xstride ), _ystride( ystride ),
		  _endian( e ),
		  _bits( bits ),
		  _floating( isfloat ),
		  _unsigned( isunsign )
	{
		precondition( offset % 8 == 0, "offset should be multiple of 8 bits" );
		precondition( ( isfloat && ! isunsign ) || ( ! isfloat ), "float must be signed" );
	}

	image_buffer( const std::shared_ptr<void> &data, int16_t bits, int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t xstride, int64_t ystride, int64_t offset, base::endianness e, bool isfloat, bool isunsign )
		: _data( data ),
		  _offset( offset ),
		  _region( rect::from_points( x1, y1, x2, y2 ) ),
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

	constexpr inline int64_t x1( void ) const { return _region.x1(); }
	constexpr inline int64_t y1( void ) const { return _region.y1(); }
	constexpr inline int64_t x2( void ) const { return _region.x2(); }
	constexpr inline int64_t y2( void ) const { return _region.y2(); }

	constexpr inline int64_t width( void ) const { return _region.width(); }
	constexpr inline int64_t height( void ) const { return _region.height(); }

	constexpr inline const rect &active_area( void ) const { return _region; }

	void reset_position( int64_t x1, int64_t y1 ) { _region.set_x( x1 ); _region.set_y( y1 ); }
	/// NB: be careful when you do this. provided as an optimization so buffers don't have
	/// to be re-created, but caveat emptor
	void force_width( int64_t w ) { _region.set_width( w ); }
	/// NB: be careful when you do this. provided as an optimization so buffers don't have
	/// to be re-created, but caveat emptor
	void force_height( int64_t h ) { _region.set_height( h ); }
	constexpr inline int64_t bits( void ) const { return _bits; }

	constexpr inline bool is_floating( void ) const { return _floating; }

	constexpr inline int64_t xstride_bytes( void ) const { return _xstride / 8; }
	constexpr inline int64_t ystride_bytes( void ) const { return _ystride / 8; }

	void get_scanline( int64_t y, float *line, int64_t stride = 1 ) const;

	void set_scanline( int64_t y, const float *line, int64_t stride = 1 );

	void set_offset( int64_t offset_bits )
	{
		precondition( offset_bits % 8 == 0, "offset should be multiple of 8 bits" );
		_offset = offset_bits;
	}
	constexpr inline int64_t offset( void ) const { return _offset; }

	inline void set_endianness( base::endianness e ) { _endian = e; }
	constexpr inline base::endianness endianness( void ) const { return _endian; }

	const std::shared_ptr<void> &raw( void ) const { return _data; }

	void *data( void )
	{
		return static_cast<char *>( _data.get() ) + offset() / 8;
	}

	const void *data( void ) const
	{
		return static_cast<const char *>( _data.get() ) + offset() / 8;
	}

	void *row( int64_t y )
	{
		uint8_t *data = static_cast<uint8_t *>( _data.get() );
		data += ( offset() + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 8;
		return data;
	}

	const void *row( int64_t y ) const
	{
		const uint8_t *data = static_cast<const uint8_t *>( _data.get() );
		data += ( offset() + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 8;
		return data;
	}

	template<typename T>
	static image_buffer simple_buffer( int64_t x1, int64_t y1, int64_t x2, int64_t y2 )
	{
		rect barea = rect::from_points( x1, y1, x2, y2 );

		// TBD: make each line sse (avx2) 256-bit (32-byte) aligned? we would have to
		// adjust opengl texturing and a few other places if we do that, and wouldn't be useable
		// on formats that just read into a flat memory buffer...
		size_t linebytes = static_cast<size_t>( barea.width() ) * sizeof(T);
		int16_t bits = static_cast<int16_t>( sizeof(T) * 8 );
		int64_t xstride = static_cast<int64_t>( sizeof(T) );
		int64_t ystride = static_cast<int64_t>( linebytes );
		constexpr bool isf = std::is_floating_point<T>::value;
		constexpr bool isu = std::is_unsigned<T>::value;

		std::shared_ptr<void> vdata = std::shared_ptr<uint8_t>(
			new uint8_t[static_cast<size_t>( ystride * barea.height() )],
			base::array_deleter<uint8_t>() );
		return image_buffer( vdata, bits, barea, xstride * 8, ystride * 8, 0,
							 base::endianness::NATIVE, isf, isu );
	}

	template<typename T>
	static image_buffer simple_interleaved( int64_t w, int64_t h, int chans, base::endianness endi = base::endianness::NATIVE )
	{
		rect barea{ 0, 0, w, h };
		// TBD: make each line sse (avx2) 256-bit (32-byte) aligned? we would have to
		// adjust opengl texturing and a few other places if we do that, and wouldn't be useable
		// on formats that just read into a flat memory buffer...
		int16_t bits = static_cast<int16_t>( sizeof(T) * 8 );
		size_t linebytes = static_cast<size_t>( w * chans ) * sizeof(T);
		int64_t xstride = static_cast<int64_t>( sizeof(T) ) * chans;
		int64_t ystride = static_cast<int64_t>( linebytes );
		constexpr bool isf = std::is_floating_point<T>::value;
		constexpr bool isu = std::is_unsigned<T>::value;

		std::shared_ptr<void> vdata = std::shared_ptr<uint8_t>(
			new uint8_t[static_cast<size_t>( ystride * h )],
			base::array_deleter<uint8_t>() );
		return image_buffer( vdata, bits, barea, xstride * 8, ystride * 8, 0, endi, isf, isu );
	}

	static image_buffer full_plane( int64_t x1, int64_t y1, int64_t x2, int64_t y2,
									int8_t bits, int8_t xsubsamp_shift, int8_t ysubsamp_shift,
									bool isfloat, bool isunsigned )
	{
		rect barea = rect::from_points( x1, y1, x2, y2 );
		int64_t bytes = static_cast<int64_t>( ( int64_t( bits ) + 7 ) / 8 );
		int64_t ystride = ( barea.width() * bytes ) >> int64_t( xsubsamp_shift );
		size_t sz = static_cast<size_t>( ystride * ( barea.height() >> int64_t( ysubsamp_shift ) ) );

		std::shared_ptr<void> vdata = std::shared_ptr<uint8_t>(
			new uint8_t[sz], base::array_deleter<uint8_t>() );
		return image_buffer( vdata, int16_t( bits ), barea, bits, ystride * 8, 0,
							 base::endianness::NATIVE, isfloat, isunsigned );
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
	rect _region = {};
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
