// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "async_io.h"
#ifdef __linux__
# include <unistd.h>
# include <sys/syscall.h>
# include <linux/aio_abi.h>
# include <cerrno>
# include <thread>
# include <mutex>
# include <map>
# include "contract.h"
#endif

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

#ifdef __linux__
inline int io_setup( int maxev, aio_context_t *ctx )
{
#ifdef __x86_64__
    long res;
    __asm__ volatile( "syscall"
                      : "=a" (res)
                      : "0" (__NR_io_setup), "D" ((long)maxev), "S" ((long)ctx)
                      : "r11","rcx","memory"
                      );
    return res;
#else
    return syscall( __NR_io_setup, (long)maxev, (long)ctx );
#endif
}

inline int io_destroy( aio_context_t ctx )
{
#ifdef __x86_64__
    long res;
    __asm__ volatile( "syscall"
                      : "=a" (res)
                      : "0" (__NR_io_destroy), "D" ((long)ctx)
                      : "r11","rcx","memory"
                      );
    return res;
#else
    return syscall( __NR_io_destroy, (long)ctx );
#endif
}

inline int io_submit( aio_context_t ctx, long nr, struct iocb **cbs )
{
#ifdef __x86_64__
    long res;
    __asm__ volatile( "syscall"
                      : "=a" (res)
                      : "0" (__NR_io_submit), "D" ((long)ctx), "S" ((long)nr), "d" ((long)cbs)
                      : "r11","rcx","memory"
                      );
    return res;
#else
    return syscall( __NR_io_submit, (long)ctx, (long)nr, (long)cbs );
#endif
}

inline int io_cancel( aio_context_t ctx, struct iocb *cb, struct io_event *ev )
{
#ifdef __x86_64__
    long res;
    __asm__ volatile( "syscall"
                      : "=a" (res)
                      : "0" (__NR_io_cancel), "D" ((long)ctx), "S" ((long)cb), "d" ((long)ev)
                      : "r11","rcx","memory"
                      );
    return res;
#else
    return syscall( __NR_io_cancel, (long)ctx, (long)cb, (long)ev );
#endif
}

inline int io_getevents( aio_context_t ctx, long min_nr, long nr, struct io_event *evs, struct timespec *tout )
{
#ifdef __x86_64__
    long res;
    __asm__ volatile( "movq %5,%%r10 ; movq %6,%%r8 ; syscall"
                      : "=a" (res)
                      : "0" (__NR_io_getevents), "D" ((long)ctx), "S" ((long)min_nr), "d" ((long)nr), "g" ((long)evs), "g" ((long)tout)
                      : "r11","rcx","memory","r8","r10"
                      );
    return res;
#else
    return syscall( __NR_io_getevents, (long)ctx, (long)min_nr, (long)nr, (long)evs, (long)tout );
#endif
}

class async_io::context
{
public:
    context( void )
        : _pfd(-1) // eventually eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK|EFD_SEMAPHORE)
        , _ctxt(0)
        , _max_live( 0 )
    {
        int nev = static_cast<int>( std::thread::hardware_concurrency() );
        nev = std::max( nev * 2, 2 );
        if ( io_setup( nev, &_ctxt ) < 0 )
            throw_errno( "Unable to create asynchronous I/O kernel buffers" );
        _max_live = static_cast<uint64_t>( nev );
    }

    ~context( void )
    {
        io_destroy( _ctxt );
        if ( _pfd )
            ::close( _pfd );
    }

    bool submit_read( uint64_t id, void *outbuf, size_t bytes, int fd, ssize_t offset )
    {
        struct iocb d = {};
        d.aio_data = id;
        d.aio_lio_opcode = IOCB_CMD_PREAD;
        d.aio_reqprio = 0;
        d.aio_fildes = fd;
        d.aio_buf = (__u64)outbuf;
        d.aio_nbytes = bytes;
        d.aio_offset = offset;
        if ( _pfd >= 0 )
        {
            d.aio_flags = IOCB_FLAG_RESFD;
            d.aio_resfd = _pfd;
        }
        struct iocb *ds[] = { &d };
        int n = 0;
        do
        {
            n = io_submit( _ctxt, 1, ds );
            if ( n < 0 )
            {
                if ( errno == EAGAIN )
                {
                    struct io_event ev = {};
                    int nr = io_getevents( _ctxt, 0, 1, &ev, NULL );
                    if ( nr < 0 )
                    {
                        if ( errno != EINTR )
                            throw_errno( "Unable to query event" );
                    }
                    else if ( nr == 1 )
                    {
                        std::lock_guard<std::mutex> lk( _mutex );
                        _completed[ev.data] = ev.res;
                    }
                    continue;
                }
                else
                    throw_errno( "Unable to submit asynchronous I/O read request" );
            }
        }
        while ( false );

        return 1 == n;
    }

    bool finished( uint64_t id, ssize_t &retval )
    {
        {
            std::lock_guard<std::mutex> lk( _mutex );
            auto i = _completed.find( id );
            if ( i != _completed.end() )
            {
                retval = i->second;
                _completed.erase( i );
                return true;
            }
        }
        do
        {
            struct io_event ev = {};
            struct timespec tout = {0, 0};
            int nr = io_getevents( _ctxt, 0, 1, &ev, &tout );
            if ( nr < 0 )
            {
                if ( errno == EINTR )
                    continue;
                throw_errno( "Unable to retrieve async events" );
            }
            else if ( nr == 1 )
            {
                if ( ev.data == id )
                {
                    retval = ev.res;
                    return true;
                }
                std::lock_guard<std::mutex> lk( _mutex );
                _completed[ev.data] = ev.res;
                break;
            }
        } while ( false );

        return false;
    }

private:
    int _pfd = -1; // todo: when we add polling and true async, we can use IOCB_FLAG_RESFD
    aio_context_t _ctxt = 0;
    std::mutex _mutex;
    std::map<uint64_t, ssize_t> _completed;
    uint64_t _max_live = 0;
};
#endif

////////////////////////////////////////

async_io::async_io( void )
#ifdef __linux__
    : _request_id( 1 ),
      _ctxt( new context )
#endif
{
}

////////////////////////////////////////

async_io::~async_io( void )
{
}

////////////////////////////////////////

#ifdef __linux__
bool async_io::submit_read( uint64_t id, void *outbuf, size_t bytes, int fd, ssize_t offset )
{
    return _ctxt->submit_read( id, outbuf, bytes, fd, offset );
}

////////////////////////////////////////

bool async_io::finished( uint64_t id, ssize_t &retval )
{
    return _ctxt->finished( id, retval );
}
#endif

} // base



