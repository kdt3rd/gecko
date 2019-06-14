// Copyright (c) 2015 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "base_streambuf.h"

////////////////////////////////////////

namespace
{
static const int kStreamBufID = std::ios_base::xalloc();

static void base_streambuf_callback(
    std::ios_base::event evt, std::ios_base &ios, int index )
{
    if ( evt == std::ios_base::erase_event )
    {
        void *ptr = ios.pword( kStreamBufID );
        if ( ptr )
        {
            base::base_streambuf *bsb =
                reinterpret_cast<base::base_streambuf *>( ptr );
            bsb->close();
            ios.pword( kStreamBufID ) = nullptr;
            delete bsb;
        }
    }
}

} // namespace

////////////////////////////////////////

namespace base
{
////////////////////////////////////////

base_streambuf::base_streambuf( std::streamsize bufSz )
    : _cur_offset( 0 ), _buf_sz( bufSz )
{
    // hrm, should we allocate something more intelligent than bufSz +
    // 1 - if the buffer size is selected to be a page (i.e. 4096
    // bytes), seems kind of wasteful to fill another page with a
    // single byte. But maybe there's few enough of these objects
    // that it just doesn't matter
    _buf_store.reset( new char_type[_buf_sz + 1] );
    _buf = _buf_store.get();
}

////////////////////////////////////////

base_streambuf::~base_streambuf( void ) {}

////////////////////////////////////////

// STATIC
void base_streambuf::register_streambuf( std::stream &s, base_streambuf *sb )
{
    s.pword( kStreamBufID ) = sb;
    s.register_callback( &base_streambuf_callback, 0 );
    s.rdbuf( sb );
}

////////////////////////////////////////

std::streambuf *base_streambuf::setbuf( char_type *s, std::streamsize n )
{
    int sn = sync();
    if ( sn != 0 )
    {
        // do we care?
    }

    _buf_sz = n;

    if ( s == nullptr )
    {
        _buf_store.reset( new char_type[n + 1] );
        _buf = _buf_store.get();
    }
    else
    {
        _buf_store.reset();
        _buf = s;
    }

    setg( _buf, _buf + _buf_sz, _buf + _buf_sz );
    setp( _buf, _buf + _buf_sz );

    return this;
}

////////////////////////////////////////

base_streambuf::pos_type base_streambuf::seekoff(
    off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which )
{}

////////////////////////////////////////

base_streambuf::pos_type
base_streambuf::seekpos( pos_type pos, std::ios_base::openmode which )
{
    return seekoff( pos, std::ios_base::beg, which );
}

////////////////////////////////////////

int base_streambuf::sync( void )
{
    // force a write of any data in the memory buffer
    overflow( traits_type::eof() );
}

////////////////////////////////////////

std::streamsize base_streambuf::showmanyc( void )
{
    // based on the definition, there's nothing better we can
    // do generically, or is there?
    return 0;
}

////////////////////////////////////////

base_streambuf::int_type base_streambuf::underflow( void ) {}

////////////////////////////////////////

std::streamsize base_streambuf::xsgetn( char_type *s, std::streamsize count ) {}

////////////////////////////////////////

std::streamsize
base_streambuf::xsputn( const char_type *s, std::streamsize count )
{
    if ( _buf_sz == 0 )
        return write( s, count * sizeof( char_type ) );

    return std::streambuf::xsputn( s, count );
}

////////////////////////////////////////

base_streambuf::int_type base_streambuf::overflow( int_type ch )
{
    // ch will be eof when called from sync and when ending a file
    // so we shouldn't add that character to the write buffer
    if ( !traits_type::eq_int_type( ch, traits_type::eof() ) )
    {
        *pptr() = ch;
        pbump( 1 );
    }

    // things have overflowed, so write stuff out
    const char_type *outBeg  = pbase();
    std::streamsize  toWrite = pptr() - outBeg;
    if ( toWrite > 0 )
        write( outBeg, toWrite * sizeof( char_type ) );

    setp( _write_buf.get(), _write_buf.get() + _write_buf_sz );
}

////////////////////////////////////////

base_streambuf::int_type base_streambuf::pbackfail( int_type c )
{
    int_type ret = traits_type::eof();
}

////////////////////////////////////////

} // namespace base
