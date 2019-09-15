// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "contract.h"
#include "streambuf.h"

#include <cerrno>
#ifdef __linux__
#    include "unix_streambuf.h"

#    include <atomic>
#    include <memory>
#elif ( defined( __MACH__ ) && defined( __APPLE__ ) ) ||                       \
    defined( __FreeBSD__ ) || defined( __NetBSD__ ) || defined( __OpenBSD__ )
#    include <unistd.h>
#    define GK_ASYNC_USE_KQUEUE 1
#endif

////////////////////////////////////////

namespace base
{

///
/// @brief Class async_io provides...
///
class async_io
{
public:
    /// this may need to create a kernel-side tracking structure so
    /// should not be considered trivial to instantiate
    async_io( void );
    ~async_io( void );
    async_io( const async_io & ) = delete;
    async_io &operator=( const async_io & ) = delete;
    /// todo: implement move ctor
    async_io( async_io && ) = delete;
    async_io &operator=( async_io && ) = delete;

    template <typename F, typename C>
    ssize_t idle_read(
        F &&               idlefunc,
        void *             outbuf,
        size_t             bytes,
        base_streambuf<C> *sbuf,
        ssize_t            fileoff )
    {
        if ( bytes == 0 )
            return 0;

        precondition( outbuf, "invalid output buffer provided" );
        precondition( sbuf, "invalid streambuf object" );
        precondition( fileoff >= 0, "invalid file offset value {0}", fileoff );
#ifdef __linux__
        auto ubuf = dynamic_cast<unix_streambuf *>( sbuf );
        if ( ubuf )
        {
            uint64_t id = generate_request_id();
            if ( submit_read(
                     id, outbuf, bytes, ubuf->get_native_handle(), fileoff ) )
            {
                ssize_t retval = -1;
                do
                {
                    std::forward<F>( idlefunc )();
                } while ( !finished( id, retval ) );
                return retval;
            }
        }
#elif defined( GK_ASYNC_USE_KQUEUE )
        // TODO
        std::forward<F>( idlefunc )();
#endif
        if ( fileoff == sbuf->pubseekoff(
                            fileoff, std::ios_base::beg, std::ios_base::in ) )
        {
            return sbuf->sgetn(
                reinterpret_cast<C *>( outbuf ), bytes / sizeof( C ) );
        }
        errno = ESPIPE;
        return -1;
    }

private:
#ifdef __linux__
    uint64_t generate_request_id( void ) { return _request_id.fetch_add( 1 ); }
    bool     submit_read(
            uint64_t id, void *outbuf, size_t bytes, int fd, ssize_t offset );
    bool finished( uint64_t id, ssize_t &retval );

    std::atomic<uint64_t> _request_id;
    struct context;
    std::unique_ptr<context> _ctxt;
#endif
};

} // namespace base
