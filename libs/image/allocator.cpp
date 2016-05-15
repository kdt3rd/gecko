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

	++_cur_buffers_live;
	_max_buffers_live = std::max( _max_buffers_live, _cur_buffers_live );
	return std::shared_ptr<void>( p, std::bind( &allocator::return_misc, this, std::placeholders::_1 ) );
}

////////////////////////////////////////

std::shared_ptr<float>
allocator::scanline( int &stride, int w )
{
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

	++_cur_buffers_live;
	_max_buffers_live = std::max( _max_buffers_live, _cur_buffers_live );
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
			if ( (*i).w < w || (*i).w > (w * 2) )
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
				p = (*i).ptr;
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
		m.size = bytes;
		m.skip_count = 0;
		_inflight_buf.emplace_back( std::move( m ) );
	}

	++_cur_buffers_live;
	_max_buffers_live = std::max( _max_buffers_live, _cur_buffers_live );
	return std::shared_ptr<float>( p, std::bind( &allocator::return_buffer, this, std::placeholders::_1 ) );
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
#else
	int s = posix_memalign( &p, align, bytes );
	if ( s != 0 )
#endif
		throw_location( std::system_error( s, std::system_category(), base::format( "Unable to allocate aligned memory of {0} bytes, aligned to {1}", bytes, align ) ) );

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
				--_cur_buffers_live;
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
				--_cur_buffers_live;
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

} // image



