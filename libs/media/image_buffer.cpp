//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <base/contract.h>
#include "image_buffer.h"

namespace media
{

////////////////////////////////////////

namespace
{

inline float convert_pel( uint8_t x )
{
	return static_cast<float>( x ) / 255.0F;
}

inline void unconvert_pel( uint8_t &d, float x )
{
	d = static_cast<uint8_t>( std::max( 0.F, std::min( 255.F, x * 255.0F + 0.5F ) ) );
}

inline float convert_pel( uint16_t x )
{
	return static_cast<float>( x ) / 65535.0F;
}

inline void unconvert_pel( uint16_t &d, float x )
{
	d = static_cast<uint16_t>( std::max( 0.F, std::min( 65535.F, x * 65535.0F + 0.5F ) ) );
}

inline float convert_pel( base::half x )
{
	return float(x);
}

inline void unconvert_pel( base::half &d, float x )
{
	d = x;
}

inline float convert_pel( float x )
{
	return x;
}

inline void unconvert_pel( float &d, float x )
{
	d = x;
}

inline float convert_pel( double x )
{
	return static_cast<float>( x );
}

inline void unconvert_pel( double &d, float v )
{
	d = static_cast<double>( v );
}

}

////////////////////////////////////////

void image_buffer::get_scanline( int64_t y, float *line, int64_t stride ) const
{
	switch ( _bits )
	{
		case 8:
			if ( _unsigned && !_floating )
				return get_scanline_u8( y, line, stride );
			break;

		case 16:
			if ( _unsigned && !_floating )
				return get_scanline_u16( y, line, stride );
			if ( !_unsigned && _floating )
				return get_scanline_f16( y, line, stride );
			break;

		case 32:
			if ( !_unsigned && _floating )
				return get_scanline_f32( y, line, stride );
			break;

		case 64:
			if ( !_unsigned && _floating )
				return get_scanline_f64( y, line, stride );
			break;
	}
	const char *uns = ( _unsigned ? "unsigned" : "signed" );
	const char *flt = ( _floating ? "float" : "integer" );
	throw_logic( "image_buffer::get_scanline not yet implemented: {0} {1} {2} bits", uns, flt, _bits );
}

////////////////////////////////////////

void image_buffer::set_scanline( int64_t y, const float *line, int64_t stride )
{
	switch ( _bits )
	{
		case 8:
			if ( _unsigned && !_floating )
				return set_scanline_u8( y, line, stride );
			break;

		case 16:
			if ( _unsigned && !_floating )
				return set_scanline_u16( y, line, stride );
			if ( !_unsigned && _floating )
				return set_scanline_f16( y, line, stride );
			break;

		case 32:
			if ( !_unsigned && _floating )
				return set_scanline_f32( y, line, stride );
			break;

		case 64:
			if ( !_unsigned && _floating )
				return set_scanline_f64( y, line, stride );
			break;
	}
	const char *uns = ( _unsigned ? "unsigned" : "signed" );
	const char *flt = ( _floating ? "float" : "integer" );
	throw_logic( "image_buffer::set_scanline not yet implemented: {0} {1} {2} bits", uns, flt, _bits );
}

////////////////////////////////////////

void image_buffer::get_scanline_u8( int64_t y, float *line, int64_t stride ) const
{
	const uint8_t *data = static_cast<const uint8_t*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 8;

	for ( int64_t x = 0, W = width(); x < W; ++x )
	{
		const uint8_t *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 8;
		line[x*stride] = convert_pel( *curData );
	}
}

////////////////////////////////////////

void image_buffer::get_scanline_u16( int64_t y, float *line, int64_t stride ) const
{
	const uint16_t *data = static_cast<const uint16_t*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 16;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const uint16_t *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			line[x*stride] = convert_pel( *curData );
		}
	}
	else
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const uint16_t *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			line[x*stride] = convert_pel( bswap_16( *curData ) );
		}
	}
}

////////////////////////////////////////

void image_buffer::get_scanline_f16( int64_t y, float *line, int64_t stride ) const
{
	const base::half *data = static_cast<const base::half*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 16;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const base::half *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			line[x*stride] = convert_pel( *curData );
		}
	}
	else
	{
		const uint16_t *udata = reinterpret_cast<const uint16_t *>( data );
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const uint16_t *curData = udata + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			line[x*stride] = convert_pel( base::half( base::half::binary, bswap_16( *curData ) ) );
		}
	}
}

////////////////////////////////////////

void image_buffer::get_scanline_f32( int64_t y, float *line, int64_t stride ) const
{
	const float *data = static_cast<const float*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 32;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const float *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 32;
			line[x*stride] = convert_pel( *curData );
		}
	}
	else
	{
		const uint32_t *udata = reinterpret_cast<const uint32_t *>( data );
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const uint32_t *curData = udata + ( ( x >> _xsubsample_shift ) * _xstride ) / 32;
			union 
			{
				uint32_t u;
				float f;
			} val;
			val.u = bswap_32( *curData );
			line[x*stride] = convert_pel( val.f );
		}
	}
}

////////////////////////////////////////

void image_buffer::get_scanline_f64( int64_t y, float *line, int64_t stride ) const
{
	const double *data = static_cast<const double*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 64;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const double *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 64;
			line[x*stride] = convert_pel( *curData );
		}
	}
	else
	{
		const uint64_t *udata = reinterpret_cast<const uint64_t *>( data );
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			const uint64_t *curData = udata + ( ( x >> _xsubsample_shift ) * _xstride ) / 64;
			union 
			{
				uint64_t u;
				double f;
			} val;
			val.u = bswap_64( *curData );
			line[x*stride] = convert_pel( val.f );
		}
	}
}

////////////////////////////////////////

void image_buffer::set_scanline_u8( int64_t y, const float *line, int64_t stride )
{
	uint8_t *data = static_cast<uint8_t*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 8;

	for ( int64_t x = 0, W = width(); x < W; ++x )
	{
		uint8_t *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 8;
		unconvert_pel( *curData, line[x*stride] );
	}
}

////////////////////////////////////////

void image_buffer::set_scanline_u16( int64_t y, const float *line, int64_t stride )
{
	uint16_t *data = static_cast<uint16_t *>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 16;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			uint16_t *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			unconvert_pel( *curData, line[x*stride] );
		}
	}
	else
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			uint16_t *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			uint16_t tmp;
			unconvert_pel( tmp, line[x*stride] );
			*curData = bswap_16( tmp );
		}
	}
}

////////////////////////////////////////

void image_buffer::set_scanline_f16( int64_t y, const float *line, int64_t stride )
{
	base::half *data = static_cast<base::half*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 16;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			base::half *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			unconvert_pel( *curData, line[x*stride] );
		}
	}
	else
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			base::half *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 16;
			base::half tmp;
			unconvert_pel( tmp, line[x*stride] );
			tmp.set_bits( bswap_16( tmp.bits() ) );
			*curData = tmp;
		}
	}
}

////////////////////////////////////////

void image_buffer::set_scanline_f32( int64_t y, const float *line, int64_t stride )
{
	float *data = static_cast<float*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 32;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			float *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 32;
			unconvert_pel( *curData, line[x*stride] );
		}
	}
	else
	{
		uint32_t *udata = reinterpret_cast<uint32_t *>( data );
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			uint32_t *curData = udata + ( ( x >> _xsubsample_shift ) * _xstride ) / 32;
			union 
			{
				uint32_t u;
				float f;
			} val;
			unconvert_pel( val.f, line[x*stride] );
			*curData = bswap_32( val.u );
		}
	}
}

////////////////////////////////////////

void image_buffer::set_scanline_f64( int64_t y, const float *line, int64_t stride )
{
	double *data = static_cast<double*>( _data.get() );
	data += ( _offset + ( ( y - y1() ) >> _ysubsample_shift ) * _ystride ) / 64;

	if ( _endian == base::endianness::NATIVE )
	{
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			double *curData = data + ( ( x >> _xsubsample_shift ) * _xstride ) / 64;
			unconvert_pel( *curData, line[x*stride] );
		}
	}
	else
	{
		uint64_t *udata = reinterpret_cast<uint64_t *>( data );
		for ( int64_t x = 0, W = width(); x < W; ++x )
		{
			uint64_t *curData = udata + ( ( x >> _xsubsample_shift ) * _xstride ) / 64;
			union 
			{
				uint64_t u;
				double f;
			} val;
			unconvert_pel( val.f, line[x*stride] );
			*curData = bswap_64( val.u );
		}
	}
}

////////////////////////////////////////

}

