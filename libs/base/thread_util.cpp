// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "thread_util.h"
#ifdef _WIN32
#    include <tchar.h>
#    include <windows.h>
#else
#    include <unistd.h>
#endif

#include <atomic>
#include <mutex>
#include <system_error>
#include <thread>

////////////////////////////////////////

namespace
{
#ifdef _WIN32
DWORD countSetBits( ULONG_PTR bitMask )
{
    const DWORD LSHIFT  = sizeof( ULONG_PTR ) * 8 - 1;
    ULONG_PTR   bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD       ret     = 0;

    for ( DWORD i = 0; i <= LSHIFT; ++i )
    {
        if ( bitMask & bitTest )
            ++ret;
        bitTest /= 2;
    }

    return ret;
}

int queryProcCount( void )
{
    typedef BOOL( WINAPI * LPFN_GLPI )(
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD );

    LPFN_GLPI                             glpi;
    BOOL                                  done = FALSE;
    std::unique_ptr<uint8_t[]>            buffer;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr          = nullptr;
    DWORD                                 returnLength = 0;
    // leaving the other stuff in here in
    // case it's useful in the future
    DWORD             logicalProcessorCount = 0;
    DWORD             numaNodeCount         = 0;
    DWORD             processorCoreCount    = 0;
    DWORD             processorL1CacheCount = 0;
    DWORD             processorL2CacheCount = 0;
    DWORD             processorL3CacheCount = 0;
    DWORD             processorPackageCount = 0;
    DWORD             byteOffset            = 0;
    PCACHE_DESCRIPTOR cacheDesc;

    glpi = (LPFN_GLPI)GetProcAddress(
        GetModuleHandle( TEXT( "kernel32" ) ),
        "GetLogicalProcessorInformation" );

    // logical processor information not supported, just return 1;
    if ( !glpi )
        return 1;

    while ( !done )
    {
        DWORD rc = glpi( ptr, &returnLength );

        if ( FALSE == rc )
        {
            DWORD eVal = GetLastError();
            if ( eVal == ERROR_INSUFFICIENT_BUFFER )
            {
                buffer.reset( new uint8_t[returnLength] );
                ptr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(
                    buffer.get() );
            }
            else
                throw std::system_error(
                    std::error_code( eVal, std::system_category() ),
                    "Unable to query logical processor information" );
        }
        else
            done = TRUE;
    }

    while ( byteOffset + sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) <=
            returnLength )
    {
        switch ( ptr->Relationship )
        {
            case RelationNumaNode:
                // Non-NUMA systems report a single record of this type.
                numaNodeCount++;
                break;

            case RelationProcessorCore:
                processorCoreCount++;

                // A hyperthreaded core supplies more than one logical processor.
                logicalProcessorCount += countSetBits( ptr->ProcessorMask );
                break;

            case RelationCache:
                // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
                cacheDesc = &ptr->Cache;
                if ( cacheDesc->Level == 1 )
                    ++processorL1CacheCount;
                else if ( cacheDesc->Level == 2 )
                    ++processorL2CacheCount;
                else if ( cacheDesc->Level == 3 )
                    ++processorL3CacheCount;
                break;

            case RelationProcessorPackage:
                // Logical processors share a physical package.
                ++processorPackageCount;
                break;

            default: break;
        }

        byteOffset += sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
        ++ptr;
    }
    return logicalProcessorCount;
}
#endif

static std::atomic<long> theCoreCount( 0 );
static std::atomic<long> theOverrideCoreCount( -1 );
std::once_flag           theInitFlag;

void initCount( void )
{
    long nThreads = static_cast<long>( std::thread::hardware_concurrency() );

    // some compiler implementations return 0, in which case, use the
    // O.S. API available
    if ( nThreads == 0 )
    {
#if defined( _SC_NPROCESSORS_ONLN )
        nThreads = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined( _SC_NPROCESSORS_CONF )
        nThreads = sysconf( _SC_NPROCESSORS_CONF );
#endif
#ifdef _WIN32
        nThreads = queryProcCount();
#endif
    }

    if ( nThreads < 0 )
        nThreads = 0;

    theCoreCount = nThreads;
}

} // namespace

////////////////////////////////////////

namespace base
{
namespace thread
{
long core_count( void )
{
    std::call_once( theInitFlag, initCount );
    long overCnt = theOverrideCoreCount;
    if ( overCnt >= 0 )
        return overCnt;
    return theCoreCount;
}

////////////////////////////////////////

void override_core_count( long cnt ) { theOverrideCoreCount = cnt; }

////////////////////////////////////////

} // namespace thread
} // namespace base
