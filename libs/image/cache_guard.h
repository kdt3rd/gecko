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

////////////////////////////////////////

namespace image
{

/// @brief RAII object to bind a cache-able buffer
template <typename CacheObject>
class cache_guard
{
public:
	inline cache_guard( void ) noexcept
		: _co( nullptr )
	{}
	explicit inline cache_guard( CacheObject &c )
		: _co( std::addressof( c ) )
	{
		bind();
	}
	inline ~cache_guard( void )
	{
		unbind();
	}
	inline cache_guard( cache_guard &&g ) noexcept
		: _co( g._co ), _owns( g._owns )
	{
		g._co = nullptr;
		g._owns = false;
	}

	inline cache_guard &operator=( cache_guard &&g ) noexcept
	{
		unbind();

		cache_guard( std::move( g ) ).swap( *this );

		g._co = nullptr;
		g._owns = false;

		return *this;
	}

	inline void bind( void ) noexcept
	{
		if ( _co && ! _owns )
		{
			_co->bind();
			_owns = true;
		}
	}
	inline void unbind( void ) noexcept
	{
		if ( _co && _owns )
		{
			_co->unbind();
			_owns = false;
		}
	}

	inline void swap( cache_guard &g ) noexcept
	{
		std::swap( g._co, _co );
		std::swap( g._owns, _owns );
	}

	inline bool owns( void ) const noexcept
	{
		return _owns;
	}

	inline CacheObject *object( void ) const noexcept
	{
		return _co;
	}

private:
	cache_guard( const cache_guard & ) = delete;
	cache_guard &operator=( const cache_guard & ) = delete;

	CacheObject *_co;
	bool _owns = false;
};

} // namespace image



