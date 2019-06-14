// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <initializer_list>
#include <ostream>
#include <utility>

////////////////////////////////////////

namespace base
{
///
/// @brief cpu provides information about the processing
///        features of the underlying CPU
///
/// This can be queried at runtime, enabling chunkjs of code
/// differently compiled to target specific CPU features to be run.
///
/// This is inspired by a merging of a few cpuid-related functions
/// found on the web.
///
struct cpu
{
    enum class simd_feature : uint8_t
    {
        NONE = 0,
        // intel SIMD features
        // ignore MMX, MMX_EXT, 3DNow!, etc. as they are very old until
        // someone ports this to a legacy platform
        SSE,
        SSE2,
        SSE3,
        SSE41,
        SSE42,
        AVX,
        AVX2,
        AVX512F,
        // new AVX512 subsets????

        // ARM SIMD
        NEON
    };

    static void output( std::ostream &os );

    /// returns the CPU vendor
    static const char *vendor( void );
    /// returns the CPU brand
    static const char *brand( void );

    static bool is_intel( void );
    static bool is_amd( void );

    static size_t cache_line_bits( void );
    /// L1 instruction cache size
    static size_t l1_instruction_cache( void );
    static size_t l1_instruction_line_size( void );
    static size_t l1_instruction_associativity( void );
    /// L1 data cache size
    static size_t l1_data_cache( void );
    static size_t l1_data_line_size( void );
    static size_t l1_associativity( void );
    /// L2 data cache size
    static size_t l2_data_cache( void );
    static size_t l2_data_line_size( void );
    static size_t l2_associativity( void );
    /// L3 data cache size
    static size_t l3_data_cache( void );
    static size_t l3_data_line_size( void );
    static size_t l3_associativity( void );

    /// prefetch size (bytes)
    static size_t prefetch( void );

    /// returns true if any SIMD features are available
    static bool has_simd( void );
    /// returns the highest / newest SIMD feature available
    static simd_feature highest_simd( void );

    /// Whether the processor supports fast unaligned loads, where an
    /// unaligned load is the same speed on an aligned memory address
    /// as an aligned load
    static bool fast_unaligned_load( void );

    /// Whether the processor supports misaligned SSE instructions
    static bool has_misaligned_sse( void );

    /// @defgroup Intel specific SIMD feature tests
    /// @{
    static bool has_MMX( void );
    static bool has_MMX_EXT( void );
    static bool has_SSE( void );
    static bool has_SSE2( void );
    static bool has_SSE3( void );
    static bool has_SSSE3( void );
    static bool has_SSE41( void );
    static bool has_SSE42( void );
    static bool has_SSE4a( void );
    static bool has_3DNOW( void );
    static bool has_3DNOW_EXT( void );
    static bool has_AVX( void );
    static bool has_AVX2( void );
    static bool has_AVX512F( void );
    static bool has_AVX512DQ( void );
    static bool has_AVX512PF( void );
    static bool has_AVX512ER( void );
    static bool has_AVX512CD( void );
    static bool has_AVX512BW( void );
    static bool has_AVX512VL( void );
    static bool has_AVX512BMI( void );
    /// returns true if the CPU has fused multiply-add
    static bool has_FMA( void );
    static bool has_FMA4( void );

    /// @}

    static bool has_NEON( void );

    /// @defgroup Miscellaneous Intel specific instructions
    /// @{
    static bool has_PCLMULQDQ( void );
    static bool has_MONITOR( void );
    static bool has_CMPXCHG16B( void );
    /// perfmon and debug capability
    static bool has_PDCM( void );
    /// direct cache access (prefetch from mmap device)
    static bool has_DCA( void );
    static bool has_MOVBE( void );
    static bool has_POPCNT( void );
    static bool has_AES( void );
    static bool has_XSAVE( void );
    static bool has_OSXSAVE( void );
    static bool has_F16C( void );
    static bool has_RDRAND( void );

    static bool has_MSR( void );
    static bool has_CX8( void );
    static bool has_SEP( void );
    static bool has_CMOV( void );
    static bool has_CLFSH( void );
    static bool has_FXSR( void );

    static bool has_FSGSBASE( void );
    static bool has_BMI1( void );
    static bool has_HLE( void );
    static bool has_BMI2( void );
    static bool has_ERMS( void );
    static bool has_INVPCID( void );
    static bool has_RTM( void );
    static bool has_RDSEED( void );
    static bool has_ADX( void );
    static bool has_SHA( void );

    static bool has_AMD_PREFETCHW( void );
    static bool has_PREFETCHWT1( void );

    static bool has_LAHF( void );
    static bool has_LZCNT( void );
    static bool has_ABM( void );
    static bool has_XOP( void );
    static bool has_TBM( void );

    static bool has_SYSCALL( void );
    static bool has_RDTSCP( void );
    /// @}
};

template <typename Functor> inline Functor choose_runtime( Functor base_func )
{
    return base_func;
}

template <typename Functor>
inline Functor choose_runtime(
    Functor base_func,
    std::initializer_list<std::pair<cpu::simd_feature, Functor>>
        func_specializer )
{
    cpu::simd_feature active = cpu::highest_simd();
    cpu::simd_feature best   = cpu::simd_feature::NONE;
    Functor           ret    = base_func;
    for ( auto &i: func_specializer )
    {
        // can't have anything better than this
        if ( i.first == active )
            return i.second;

        if ( i.first < active && i.first > best )
        {
            best = i.first;
            ret  = i.second;
        }
    }

    return ret;
}

} // namespace base
