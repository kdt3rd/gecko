// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

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



