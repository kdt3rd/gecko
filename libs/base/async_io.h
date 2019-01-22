//
// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cerrno>
#include "streambuf.h"
#ifdef __linux__
#include <atomic>
#include <memory>
#include "unix_streambuf.h"
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
    ssize_t idle_read( F &&idlefunc, void *outbuf, size_t bytes,
                       base_streambuf<C> &sbuf, ssize_t fileoff )
    {
#ifdef __linux__
        auto ubuf = dynamic_cast<unix_streambuf *>( &sbuf );
        if ( ubuf )
        {
            uint64_t id = generate_request_id();
            if ( submit_read( id, outbuf, bytes, ubuf->get_native_handle(), fileoff ) )
            {
                ssize_t retval = -1;
                do
                {
                    std::forward<F>(idlefunc)();
                } while ( ! finished( id, retval ) );
                return retval;
            }
        }
#endif
        if ( fileoff == static_cast<size_t>( sbuf.pubseekoff( fileoff, std::ios_base::beg, std::ios_base::in ) ) )
        {
            return sbuf.sgetn( reinterpret_cast<C *>( outbuf ), bytes / sizeof(C) );
        }
        errno = ESPIPE;
        return -1;
    }
private:
#ifdef __linux__
    uint64_t generate_request_id( void ) { return _request_id.fetch_add( 1 ); }
    bool submit_read( uint64_t id, void *outbuf, size_t bytes, int fd, ssize_t offset );
    bool finished( uint64_t id, ssize_t &retval );

    std::atomic<uint64_t> _request_id;
    struct context;
    std::unique_ptr<context> _ctxt;
#endif
};

} // namespace base



