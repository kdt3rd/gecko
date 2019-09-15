// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "threading.h"

#include <atomic>
#include <base/contract.h>
#include <base/thread_util.h>

////////////////////////////////////////

namespace
{
static std::shared_ptr<image::threading> theThreadObj;
std::once_flag                           initThreadingFlag;

static void shutdownThreading( void )
{
    if ( theThreadObj )
    {
        theThreadObj->shutdown();
        theThreadObj.reset();
    }
}

static void initThreading( int count )
{
    if ( count >= 0 )
        theThreadObj = std::make_shared<image::threading>( count );
    else
        theThreadObj =
            std::make_shared<image::threading>( base::thread::core_count() );
    std::atexit( shutdownThreading );
}

} // namespace

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

threading::threading( int tCount ) : _avail_workers( nullptr ), _count( tCount )
{
    size_t n = 0;
    if ( tCount > 0 )
    {
        n = static_cast<size_t>( tCount );
        //std::cout << "Starting " << n << " threads for image processing..." << std::endl;
        //		std::lock_guard<std::mutex> lk( std::mutex );
        _threads.resize( n );
        for ( size_t i = 0; i != n; ++i )
        {
            _threads[i].reset( new worker_bee );
            put_back( _threads[i].get() );
        }
    }
}

////////////////////////////////////////

threading::~threading( void ) {}

////////////////////////////////////////

void threading::dispatch(
    const std::function<void( size_t, int, int )> &f, int start, int N )
{
    precondition(
        N > 0, "attempt to dispatch with no items ({0}) to process", N );

    // TODO: do we want to oversubscribe a bit, or only dispatch n-1
    // threads?  right now, it's simpler to oversubscribe, although we
    // could adjust count if that turns to be an issue

    //	int nPer = ( N + ( nT - 1 ) ) / nT;
    int nPer = ( N + _count ) / ( _count + 1 );
    int curS = start;
    int endV = start + N;

    worker_bee *workers = nullptr;
    worker_bee *avail   = nullptr;
    size_t      curIdx  = 0;
    while ( curS < endV )
    {
        int chunkE = curS + nPer;
        if ( chunkE > endV )
        {
            // if we didn't use all that we stole, put them back
            // before we do the last chunk we kept to do while we wait
            // for others
            if ( avail )
            {
                put_back( avail );
                avail = nullptr;
            }
            chunkE = endV;
            f( curIdx, curS, chunkE );
            break;
        }

        if ( !avail )
            avail = try_steal();

        if ( avail )
        {
            worker_bee *b    = avail;
            worker_bee *next = avail->_next.load( std::memory_order_relaxed );
            avail            = next;
            b->_next.store( workers, std::memory_order_relaxed );
            b->_index = curIdx;
            b->_start = curS;
            b->_end   = chunkE;
            b->_func  = &f;
            b->_finished.store( false, std::memory_order_relaxed );
            b->_sema.signal();
            workers = b;
        }
        else
        {
            // no threads available currently, run a chunk...
            f( curIdx, curS, chunkE );
        }
        curS = chunkE;
        ++curIdx;
    }

    if ( avail )
    {
        put_back( avail );
        avail = nullptr;
    }

    // TODO: Add a contribution model where we can process other
    // people's requests... should be able to make a custom
    // loop that injects ourself into the pool, but only does
    // a single sem_timedwait and then returns to check this...
    while ( workers )
    {
        worker_bee *cur  = workers;
        worker_bee *prev = nullptr;
        while ( cur )
        {
            if ( cur->_finished.load( std::memory_order_relaxed ) )
            {
                worker_bee *next = cur->_next.load( std::memory_order_relaxed );
                cur->_next.store( nullptr, std::memory_order_relaxed );
                put_back( cur );
                if ( cur == workers )
                    workers = next;
                else if ( prev )
                    prev->_next.store( next, std::memory_order_relaxed );
                cur = next;
            }
            else
            {
                prev = cur;
                cur  = cur->_next.load( std::memory_order_relaxed );
            }
        }
        // HRM, how do we safely get this out of the list to loop around again?
        //		if ( workers )
        //		{
        //			// things still in flight, donate some work
        //			worker_bee wb( std::this_thread::get_id() );
        //			donate( wb );
        //		}
    }
}

////////////////////////////////////////

void threading::shutdown( void )
{
    _avail_workers.store( nullptr, std::memory_order_relaxed );
    for ( auto &t: _threads )
    {
        t->_shutdown.store( true, std::memory_order_relaxed );
        t->_sema.signal();
        t->_thread.join();
    }
    _threads.clear();
}

////////////////////////////////////////

threading &threading::get( int count )
{
    std::call_once( initThreadingFlag, initThreading, count );

    return *( theThreadObj );
}

////////////////////////////////////////

void threading::init( int count )
{
    std::call_once( initThreadingFlag, initThreading, count );
}

////////////////////////////////////////

void threading::donate( worker_bee & ) {}

////////////////////////////////////////

threading::worker_bee::worker_bee( void )
    : _next( nullptr ), _func( nullptr ), _finished( true ), _shutdown( false )
{
    _thread = std::thread( &threading::worker_bee::run_bee, this );
}

////////////////////////////////////////

threading::worker_bee::worker_bee( std::thread::id )
    : _next( nullptr ), _func( nullptr ), _finished( true ), _shutdown( false )
{}

////////////////////////////////////////

void threading::worker_bee::run_bee( void )
{
    while ( true )
    {
        _sema.wait();

        if ( _shutdown.load( std::memory_order_relaxed ) )
            break;

        if ( _func )
        {
            ( *_func )( _index, _start, _end );
            _func = nullptr;
            _finished.store( true, std::memory_order_relaxed );
        }
    }
}

////////////////////////////////////////

} // namespace image
