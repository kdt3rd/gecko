//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include "allocator.h"
#include <algorithm>
#ifdef __SSE__
# if defined(LINUX) || defined(__linux__)
#  include <x86intrin.h>
# else
#  include <xmmintrin.h>
#  include <immintrin.h>
# endif
#endif

////////////////////////////////////////

namespace image
{

/// @brief scanline wraps a scanline of plane pixels
///
/// implements a form of copy-on-write semantics, where if constructed
/// with a writable pointer, does not copy as other scanlines from the
/// same data are constructed, but if a const pointer is copied, will
/// be copy on write.
///
/// NB: It is expected that except when explicitly duplicated, all
/// pointers provided have a lifetime longer than the scanline class,
/// or any copies of the scanline
///
class scanline
{
public:
	scanline( void ) = default;
	scanline( const float *b, int w, int s, bool dup = false );
	scanline( float *b, int w, int s );
	scanline( int w );
	scanline( const scanline &o ) = default;
	scanline( scanline &&o ) = default;
	scanline &operator=( const scanline &o ) = default;
	scanline &operator=( scanline &&o ) = default;
	~scanline( void ) = default;

	inline bool empty( void ) const;
	inline bool is_reference( void ) const;
	inline bool unique( void ) const;

	inline int width( void ) const;
	inline int stride( void ) const;

	inline void clear( void );

	inline float *get( void );
	inline const float *get( void ) const;

	inline float &operator[]( int x );
	inline float operator[]( int x ) const;

#if defined(__SSE__)
	inline int chunks4( void ) const;
	inline __m128 load4( int chunk ) const;
	inline void store4( __m128 v, int chunk );
#endif
#if defined(__AVX__)
	inline int chunks8( void ) const;
	inline __m256 load8( int chunk ) const;
	inline void store8( __m256 v, int chunk );
#endif
#if defined(__AVX512F__)
	inline int chunks16( void ) const;
	inline __m512 load16( int chunk ) const;
	inline void store16( __m512 v, int chunk );
#endif

	inline float *begin( void );
	inline const float *begin( void ) const;
	inline const float *cbegin( void ) const;

	inline float *end( void );
	inline const float *end( void ) const;
	inline const float *cend( void ) const;

	void swap( scanline &o );

private:
	std::shared_ptr<float> _ptr;
	const float *_ref_ptr = nullptr;
	int _width = 0;
	int _stride = 0;
};

////////////////////////////////////////

inline bool
scanline::empty( void ) const
{
	return _width == 0;
}

////////////////////////////////////////

inline bool
scanline::is_reference( void ) const
{
	return !( unique() );
}

////////////////////////////////////////

inline bool
scanline::unique( void ) const
{
	return _ptr && _ptr.unique();
}

////////////////////////////////////////

inline int scanline::width( void ) const 
{
	return _width;
}

inline int scanline::stride( void ) const
{
	return _stride;
}

////////////////////////////////////////

inline void
scanline::clear( void )
{
	_ptr.reset();
	_ref_ptr = nullptr;
	_width = 0;
	_stride = 0;
}

////////////////////////////////////////

inline float *scanline::get( void )
{
	if ( ! _ptr )
	{
		_ptr = allocator::get().scanline( _stride, _width );
		if ( _ref_ptr )
			std::copy( _ref_ptr, _ref_ptr + _width, _ptr.get() );
		_ref_ptr = _ptr.get();
	}
	return _ptr.get();
}

inline const float *scanline::get( void ) const
{
	return _ref_ptr;
}

////////////////////////////////////////

inline float &
scanline::operator[]( int x )
{
	return *(get() + x);
}

inline float scanline::operator[]( int x ) const
{
	return *(get() + x);
}


#if defined(__SSE__)
inline int scanline::chunks4( void ) const
{
	return ( _width + 3 ) / 4;
}

inline __m128 scanline::load4( int chunk ) const
{
	return _mm_load_ps( get() + chunk*4 );
}

inline void scanline::store4( __m128 v, int chunk )
{
	_mm_store_ps( get() + chunk*4, v );
}

#endif
#if defined(__AVX__)
inline int scanline::chunks8( void ) const
{
	return ( _width + 7 ) / 8;
}

inline __m256 scanline::load8( int chunk ) const
{
	return _mm256_load_ps( get() + chunk * 8 );
}

inline void scanline::store8( __m256 v, int chunk )
{
	_mm256_store_ps( get() + chunk * 8, v );
}

#endif
#if defined(__AVX512F__)
inline int scanline::chunks16( void ) const
{
	return ( _width + 15 ) / 16;
}

inline __m512 scanline::load16( int chunk ) const
{
	return _mm512_load_ps( get() + chunk * 16 );
}

inline void scanline::store16( __m512 v, int chunk )
{
	_mm512_store_ps( get() + chunk * 16, v );
}
#endif

inline float *scanline::begin( void )
{
	return get();
}

inline const float *scanline::begin( void ) const
{
	return get();
}

inline const float *scanline::cbegin( void ) const
{
	return get();
}

////////////////////////////////////////

inline float *scanline::end( void )
{
	return get() + _width;
}

inline const float *scanline::end( void ) const
{
	return get() + _width;
}

inline const float *scanline::cend( void ) const
{
	return get() + _width;
}

inline void swap( scanline &a, scanline &b )
{
	a.swap( b );
}

} // namespace image

namespace std
{

inline void swap( image::scanline &a, image::scanline &b )
{
	a.swap( b );
}

}



