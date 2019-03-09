// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "allocator.h"
#ifdef _WIN32
#include <malloc.h>
#endif
#include <cstdlib>
#include <functional>
#include <iostream>
#include <base/contract.h>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

allocator::allocator( void )
{
}

////////////////////////////////////////

allocator::~allocator( void )
{
	clear_stash();
	if ( _cur_alloced > 0 )
	{
		// still have stuff in flight...
		// we can't throw here, as would be nice, just report an error
		std::cerr << "ERROR: allocator destroyed with active memory still" << std::endl;
	}
}

////////////////////////////////////////

void
allocator::set_stash_size( size_t maxBytes )
{
	std::lock_guard<std::mutex> lk( _mutex );
	_max_stash_size = maxBytes;
	reduce_stash( _max_stash_size );
}

////////////////////////////////////////

void
allocator::set_cache_limit( size_t cacheStart )
{
	std::lock_guard<std::mutex> lk( _mutex );
	_cache_start = cacheStart;
}

////////////////////////////////////////

void
allocator::set_skippiness( size_t s )
{
	std::lock_guard<std::mutex> lk( _mutex );
	_stash_skippiness = s;
}

////////////////////////////////////////

std::shared_ptr<void>
allocator::allocate( size_t bytes, size_t align )
{
	precondition( bytes != 0, "attempt to create empty buffer with 0 bytes" );
	void *p = nullptr;

	std::unique_lock<std::mutex> lk( _mutex );
	if ( ! _stash_misc.empty() )
	{
		for ( auto i = _stash_misc.begin(); i != _stash_misc.end(); )
		{
			if ( (*i).size < bytes || (*i).size > (bytes * 2) )
			{
				++((*i).skip_count);
				if ( (*i).skip_count > _stash_skippiness )
				{
					destroy( (*i).ptr, (*i).size );
					_cur_stash_size -= (*i).size;
					i = _stash_misc.erase( i );
				}
				else
				{
					++i;
				}
			}
			else
			{
				p = (*i).ptr;
				(*i).skip_count = 0;
				_cur_stash_size -= (*i).size;
				_cur_memory_live += (*i).size;
				_inflight_misc.push_front( (*i) );
				_stash_misc.erase( i );
				break;
			}
		}
	}
	if ( ! p )
	{
		p = create( lk, bytes, align, _max_misc_size );

		memMisc m;
		m.ptr = p;
		m.size = bytes;
		m.skip_count = 0;
		_inflight_misc.emplace_back( std::move( m ) );
	}

	++_cur_misc_live;
	_max_misc_live = std::max( _max_misc_live, _cur_misc_live );
	return std::shared_ptr<void>( p, std::bind( &allocator::return_misc, this, std::placeholders::_1 ) );
}

////////////////////////////////////////

std::shared_ptr<float>
allocator::scanline( int &stride, int w )
{
	precondition( w != 0, "attempt to create empty scanline" );
	float *p = nullptr;

	std::unique_lock<std::mutex> lk( _mutex );
	if ( ! _stash_scan.empty() )
	{
		for ( auto i = _stash_scan.begin(); i != _stash_scan.end(); )
		{
			if ( (*i).w < w || (*i).w > (w * 2) )
			{
				++((*i).skip_count);
				if ( (*i).skip_count > _stash_skippiness )
				{
					destroy( (*i).ptr, (*i).size );
					_cur_stash_size -= (*i).size;
					i = _stash_scan.erase( i );
				}
				else
				{
					++i;
				}
			}
			else
			{
				p = (*i).ptr;
				stride = (*i).stride;
				(*i).skip_count = 0;
				_cur_stash_size -= (*i).size;
				_cur_memory_live += (*i).size;
				_inflight_scan.push_front( (*i) );
				_stash_scan.erase( i );
				break;
			}
		}
	}

	if ( ! p )
	{
		int s = w;
		if ( ( s % floatAlignCount ) != 0 )
			s = s + ( floatAlignCount - ( s % floatAlignCount ) );
		size_t bytes = static_cast<size_t>( s ) * sizeof(float);
		p = reinterpret_cast<float *>( create( lk, bytes, defaultAlign, _max_scan_size ) );

		stride = s;

		memScan m;
		m.ptr = p;
		m.w = w;
		m.stride = s;
		m.size = bytes;
		m.skip_count = 0;
		_inflight_scan.emplace_back( std::move( m ) );
	}

	++_cur_scan_live;
	_max_scan_live = std::max( _max_scan_live, _cur_scan_live );
	return std::shared_ptr<float>( p, std::bind( &allocator::return_scan, this, std::placeholders::_1 ) );
}

////////////////////////////////////////

std::shared_ptr<float>
allocator::buffer( int &stride, int w, int h )
{
	float *p = nullptr;

	std::unique_lock<std::mutex> lk( _mutex );
	if ( ! _stash_buf.empty() )
	{
		for ( auto i = _stash_buf.begin(); i != _stash_buf.end(); )
		{
			if ( (*i).bpe != sizeof(float) || (*i).w < w || (*i).w > (w * 2) )
			{
				++((*i).skip_count);
				if ( (*i).skip_count > _stash_skippiness )
				{
					destroy( (*i).ptr, (*i).size );
					_cur_stash_size -= (*i).size;
					i = _stash_buf.erase( i );
				}
				else
				{
					++i;
				}
			}
			else
			{
				p = reinterpret_cast<float *>( (*i).ptr );
				stride = (*i).stride;
				(*i).skip_count = 0;
				_cur_stash_size -= (*i).size;
				_cur_memory_live += (*i).size;
				_inflight_buf.push_front( (*i) );
				_stash_buf.erase( i );
				break;
			}
		}
	}

	if ( ! p )
	{
		int s = w;
		if ( ( s % floatAlignCount ) != 0 )
			s = s + ( floatAlignCount - ( s % floatAlignCount ) );
		size_t bytes = static_cast<size_t>( s * h ) * sizeof(float);
		p = reinterpret_cast<float *>( create( lk, bytes, defaultAlign, _max_buffer_size ) );

		stride = s;

		memBuf m;
		m.ptr = p;
		m.w = w;
		m.h = h;
		m.stride = s;
		m.bpe = sizeof(float);
		m.size = bytes;
		m.skip_count = 0;
		_inflight_buf.emplace_back( std::move( m ) );
	}

	++_cur_buffers_live;
	_max_buffers_live = std::max( _max_buffers_live, _cur_buffers_live );
	return std::shared_ptr<float>( p, std::bind( &allocator::return_buffer, this, std::placeholders::_1 ) );
}

////////////////////////////////////////

std::shared_ptr<double>
allocator::dbl_buffer( int &stride, int w, int h )
{
	double *p = nullptr;

	std::unique_lock<std::mutex> lk( _mutex );
	if ( ! _stash_buf.empty() )
	{
		for ( auto i = _stash_buf.begin(); i != _stash_buf.end(); )
		{
			if ( (*i).bpe != sizeof(double) || (*i).w < w || (*i).w > (w * 2) )
			{
				++((*i).skip_count);
				if ( (*i).skip_count > _stash_skippiness )
				{
					destroy( (*i).ptr, (*i).size );
					_cur_stash_size -= (*i).size;
					i = _stash_buf.erase( i );
				}
				else
				{
					++i;
				}
			}
			else
			{
				p = reinterpret_cast<double *>( (*i).ptr );
				stride = (*i).stride;
				(*i).skip_count = 0;
				_cur_stash_size -= (*i).size;
				_cur_memory_live += (*i).size;
				_inflight_buf.push_front( (*i) );
				_stash_buf.erase( i );
				break;
			}
		}
	}

	if ( ! p )
	{
		int s = w;
		if ( ( s % doubleAlignCount ) != 0 )
			s = s + ( doubleAlignCount - ( s % doubleAlignCount ) );
		size_t bytes = static_cast<size_t>( s * h ) * sizeof(double);
		p = reinterpret_cast<double *>( create( lk, bytes, defaultAlign, _max_buffer_size ) );

		stride = s;

		memBuf m;
		m.ptr = p;
		m.w = w;
		m.h = h;
		m.stride = s;
		m.bpe = sizeof(double);
		m.size = bytes;
		m.skip_count = 0;
		_inflight_buf.emplace_back( std::move( m ) );
	}

	++_cur_buffers_live;
	_max_buffers_live = std::max( _max_buffers_live, _cur_buffers_live );
	return std::shared_ptr<double>( p, std::bind( &allocator::return_dbl_buffer, this, std::placeholders::_1 ) );
}

////////////////////////////////////////

void
allocator::clear_stash( void ) noexcept
{
	std::lock_guard<std::mutex> lk( _mutex );
	reduce_stash( 0 );
}

////////////////////////////////////////

void
allocator::report( std::ostream &os )
{
	std::lock_guard<std::mutex> lk( _mutex );
	os << "\nAllocator report:"
	   << "\n     Max Bytes Alloc: " << _max_alloced
	   << "\n     Cur Bytes Alloc: " << _cur_alloced
	   << "\n     Max Buffer Size: " << _max_buffer_size
	   << "\n    Max Buffers Live: " << _max_buffers_live
	   << "\n    Cur Buffers Live: " << _cur_buffers_live
	   << "\n       Max Scan Size: " << _max_scan_size
	   << "\n       Max Scan Live: " << _max_scan_live
	   << "\n       Cur Scan Live: " << _cur_scan_live
	   << "\n       Max Misc Size: " << _max_misc_size
	   << "\n       Max Misc Live: " << _max_misc_live
	   << "\n       Cur Misc Live: " << _cur_misc_live
	   << "\n      Max Stash Size: " << _max_stash_size
	   << "\n      Cur Stash Size: " << _cur_stash_size
	   << std::endl;
}

////////////////////////////////////////

allocator &allocator::get( void )
{
	static allocator globAlloc;
	return globAlloc;
}

////////////////////////////////////////

void
allocator::reduce_stash( size_t targsize )
{
	// assumes the caller holds the mutex

	// blow away random buffers first
	while ( _cur_stash_size > targsize && ! _stash_misc.empty() )
	{
		memMisc &x = _stash_misc.front();
		destroy( x.ptr, x.size );
		_cur_stash_size -= x.size;
		_stash_misc.erase( _stash_misc.begin() );
	}

	// blow away bigger ticket items next
	while ( _cur_stash_size > targsize && ! _stash_buf.empty() )
	{
		memBuf &x = _stash_buf.front();
		destroy( x.ptr, x.size );
		_cur_stash_size -= x.size;
		_stash_buf.erase( _stash_buf.begin() );
	}

	while ( _cur_stash_size > targsize && ! _stash_scan.empty() )
	{
		memScan &x = _stash_scan.front();
		destroy( x.ptr, x.size );
		_cur_stash_size -= x.size;
		_stash_scan.erase( _stash_scan.begin() );
	}
}

////////////////////////////////////////

void *
allocator::create( std::unique_lock<std::mutex> &lk, size_t bytes, size_t align, size_t &maxEntry )
{
	lk.unlock();

	void *p = nullptr;
#ifdef _WIN32
	p = _aligned_malloc( bytes, align );
	if ( p == nullptr )
		throw_location( std::system_error( errno, std::system_category(), base::format( "Unable to allocate aligned memory of {0} bytes, aligned to {1}", bytes, align ) ) );
#else
	int s = posix_memalign( &p, align, bytes );
	if ( s != 0 )
		throw_location( std::system_error( s, std::system_category(), base::format( "Unable to allocate aligned memory of {0} bytes, aligned to {1}", bytes, align ) ) );
#endif

	lk.lock();

	_cur_alloced += bytes;
	_max_alloced = std::max( _max_alloced, _cur_alloced );
	_cur_memory_live += bytes;
	_max_memory_live = std::max( _max_memory_live, _cur_memory_live );
	maxEntry = std::max( maxEntry, bytes );

	return p;
}

////////////////////////////////////////

void
allocator::destroy( void *p, size_t b )
{
	free( p );
	_cur_alloced -= b;
}

////////////////////////////////////////

void
allocator::return_misc( void *p ) noexcept
{
	if ( p )
	{
		std::lock_guard<std::mutex> lk( _mutex );
		for ( auto i = _inflight_misc.begin(); i != _inflight_misc.end(); ++i )
		{
			if ( (*i).ptr == p )
			{
				_cur_stash_size += (*i).size;
				_cur_memory_live -= (*i).size;
				--_cur_misc_live;
				_stash_misc.push_back( (*i) );
				_inflight_misc.erase( i );
				reduce_stash( _max_stash_size );
				return;
			}
		}

		std::cout << "ERROR: invalid pointer passed to return_misc" << std::endl;
	}
}

////////////////////////////////////////

void
allocator::return_scan( float *p ) noexcept
{
	if ( p )
	{
		std::lock_guard<std::mutex> lk( _mutex );
		for ( auto i = _inflight_scan.begin(); i != _inflight_scan.end(); ++i )
		{
			if ( (*i).ptr == p )
			{
				_cur_stash_size += (*i).size;
				_cur_memory_live -= (*i).size;
				--_cur_scan_live;
				_stash_scan.push_back( (*i) );
				_inflight_scan.erase( i );
				reduce_stash( _max_stash_size );
				return;
			}
		}

		std::cout << "ERROR: invalid pointer passed to return_scan" << std::endl;
	}
}

////////////////////////////////////////

void
allocator::return_buffer( float *p ) noexcept
{
	if ( p )
	{
		std::lock_guard<std::mutex> lk( _mutex );
		for ( auto i = _inflight_buf.begin(); i != _inflight_buf.end(); ++i )
		{
			if ( (*i).ptr == p )
			{
				_cur_stash_size += (*i).size;
				_cur_memory_live -= (*i).size;
				--_cur_buffers_live;
				_stash_buf.push_back( (*i) );
				_inflight_buf.erase( i );
				reduce_stash( _max_stash_size );
				return;
			}
		}

		std::cout << "ERROR: invalid pointer passed to return_buffer" << std::endl;
	}
}

////////////////////////////////////////

void
allocator::return_dbl_buffer( double *p ) noexcept
{
	if ( p )
	{
		std::lock_guard<std::mutex> lk( _mutex );
		for ( auto i = _inflight_buf.begin(); i != _inflight_buf.end(); ++i )
		{
			if ( (*i).ptr == p )
			{
				_cur_stash_size += (*i).size;
				_cur_memory_live -= (*i).size;
				--_cur_buffers_live;
				_stash_buf.push_back( (*i) );
				_inflight_buf.erase( i );
				reduce_stash( _max_stash_size );
				return;
			}
		}

		std::cout << "ERROR: invalid pointer passed to return_dbl_buffer" << std::endl;
	}
}

} // image



