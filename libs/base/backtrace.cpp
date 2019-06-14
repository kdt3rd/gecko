// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#include "backtrace.h"
#ifdef _WIN32
#    include <dbghelp.h>
#    include <iomanip>
#    include <sstream>
#    include <vector>
#    include <windows.h>
#else
#    include <execinfo.h>
#endif
#include "scope_guard.h"

#include <memory>
#include <unistd.h>

#ifdef _WIN32
int backtrace( void **stack, int n )
{
    return CaptureStackBackTrace( 0, n, stack, NULL );
}

void backtrace_symbols_fd( void *const *stack, int n, int fd )
{
    HANDLE process = GetCurrentProcess();
    SymInitialize( process, NULL, TRUE );
    std::unique_ptr<uint8_t[]> buf(
        new uint8_t[sizeof( SYMBOL_INFO ) + 256 * sizeof( char )] );
    SYMBOL_INFO *symbol  = reinterpret_cast<SYMBOL_INFO *>( buf.get() );
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

    for ( int i = 0; i < n; ++i )
    {
        SymFromAddr( process, ( DWORD64 )( stack[i] ), 0, symbol );
        std::stringstream msg;
        msg << ( n - i - 1 ) << ": " << symbol->Name << " (0x" << std::hex
            << std::setw( 8 ) << std::setfill( '0' ) << symbol->Address
            << std::dec << '\n';
        std::string outb = msg.str();
        _write( fd, outb.c_str(), outb.size() );
    }
}

char **backtrace_symbols( void *const *stack, int n )
{
    HANDLE process = GetCurrentProcess();
    SymInitialize( process, NULL, TRUE );
    std::unique_ptr<uint8_t[]> buf(
        new uint8_t[sizeof( SYMBOL_INFO ) + 256 * sizeof( char )] );
    SYMBOL_INFO *symbol  = reinterpret_cast<SYMBOL_INFO *>( buf.get() );
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

    std::string         sbuf;
    std::vector<size_t> pos;
    for ( int i = 0; i < n; ++i )
    {
        SymFromAddr( process, ( DWORD64 )( stack[i] ), 0, symbol );
        std::stringstream msg;
        msg << ( n - i - 1 ) << ": " << symbol->Name << " (0x" << std::hex
            << std::setw( 8 ) << std::setfill( '0' ) << symbol->Address
            << std::dec;
        if ( !sbuf.empty() )
            sbuf.push_back( '\0' );
        pos.push_back( sbuf.size() );
        sbuf.append( msg.str() );
    }
    size_t dataOff = sizeof( char * ) * n;
    char * memchunk =
        reinterpret_cast<char *>( malloc( dataOff + sbuf.size() + 1 ) );
    if ( !memchunk )
        throw std::runtime_error( "out of memory" );
    char *strdata = memchunk + dataOff;
    memcpy( strdata, sbuf.c_str(), sbuf.size() + 1 );
    char **retval = reinterpret_cast<char **>( memchunk );

    for ( int i = 0; i < n; ++i )
        retval[i] = strdata + pos[i];

    return retval;
}

#endif

namespace base
{
void backtrace( void )
{
    void *array[10];
    int   size = ::backtrace( array, 10 );
    ::backtrace_symbols_fd( array, size, STDERR_FILENO );
}

////////////////////////////////////////

void backtrace( std::ostream &os, size_t nLevels )
{
    std::unique_ptr<void *[]> array( new void *[nLevels] );
    int    size = ::backtrace( array.get(), static_cast<int>( nLevels ) );
    char **syms = ::backtrace_symbols( array.get(), size );
    on_scope_exit { free( syms ); };

    for ( int i = 0; i < size; ++i )
        os << syms[i] << '\n';
}

////////////////////////////////////////

void backtrace( std::vector<std::string> &l, size_t nLevels )
{
    l.clear();

    std::unique_ptr<void *[]> array( new void *[nLevels] );
    int    size = ::backtrace( array.get(), static_cast<int>( nLevels ) );
    char **syms = ::backtrace_symbols( array.get(), size );
    on_scope_exit { free( syms ); };

    for ( int i = 0; i < size; ++i )
        l.emplace_back( std::string( syms[i] ) );
}

////////////////////////////////////////

} // namespace base
