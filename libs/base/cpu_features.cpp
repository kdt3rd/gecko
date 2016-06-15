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

#include "cpu_features.h"

#include <stdint.h>

#include <bitset>
#include <vector>
#include <sstream>
#include <cstring>
#include <array>
#include <iostream>
#include <iomanip>

#ifdef _WIN32
# include <intrin.h>
#else
# include <cpuid.h>
#endif


////////////////////////////////////////


namespace
{

#if defined( __x86_64__ ) || defined( _M_X64 ) || defined( __X86__ ) || defined( __i386__ ) || defined( i386 ) || defined( _M_IX86 ) || defined( __386__ )
# define IS_INTEL_ISH 1
# ifdef _WIN32
inline unsigned int pullMaxID( unsigned int idx, std::array<unsigned int, 4> &regs )
{
	__cpuid( (int *)regs.data(), 0 );
	return regs[0];
}
inline void pullID( unsigned int idx, std::array<unsigned int, 4> &regs )
{
	__cpuidex( (int *)regs.data(), idx, 0 );
}
inline void pullID( unsigned int idx, unsigned int count, std::array<unsigned int, 4> &regs )
{
	__cpuidex( (int *)regs.data(), idx, count );
}

# else

inline void pullID( unsigned int idx, std::array<unsigned int, 4> &regs )
{
	__get_cpuid( idx, &regs[0], &regs[1], &regs[2], &regs[3] );
}
inline void pullID( unsigned int idx, unsigned int count, std::array<unsigned int, 4> &regs )
{
	__cpuid_count( idx, count, regs[0], regs[1], regs[2], regs[3] );
}
inline unsigned int pullMaxID( unsigned int idx, std::array<unsigned int, 4> &regs )
{
	pullID( idx, regs );
	return regs[0];
}
# endif
#elif defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__aarch64__)
# define IS_ARM 1
#endif

struct CPUFeatureStore
{
	CPUFeatureStore( void )
	{
#if IS_INTEL_ISH
		std::array<unsigned int, 4> regs;
		unsigned int maxFeat = pullMaxID( 0, regs );

		_amd = regs[2] == 0x444D4163;
		_intel = regs[2] == 0x6C65746E;

		// Capture vendor string
		{
			size_t dsize = sizeof(regs[1]);
			memcpy( _vendor, &(regs[1]), dsize );
			memcpy( _vendor + dsize, &(regs[3]), dsize );
			memcpy( _vendor + dsize*2, &(regs[2]), dsize );
			_vendor[12] = '\0';
		}

		if ( maxFeat >= 1 )
		{
			pullID( 1, regs );
			_feat1_ECX = regs[2];
			_feat1_EDX = regs[3];
			_proc_model = ( regs[0] >> 4 ) & 0xF;
			_proc_family = ( regs[0] >> 8 ) & 0xF;
			if ( _proc_family == 0xF )
			{
				_proc_family += ( ( regs[0] >> 16 ) & 0xFF );
				_proc_model += ( ( regs[0] >> 12 ) & 0xF );
			}
			else if ( ! _amd )
			{
				if ( _proc_family == 0x6 )
					_proc_model += ( ( regs[0] >> 12 ) & 0xF );
				else if ( _proc_family == 0xB )
				{
					_proc_family += ( ( regs[0] >> 16 ) & 0xFF );
					_proc_model += ( ( regs[0] >> 12 ) & 0xF );
				}
			}

			_cache_line_bits = ( ( regs[1] >> 8) & 0xFF ) * 8;
		}

		if ( maxFeat >= 7 )
		{
			// this feature needs count (ECX) of 0 to pull
			pullID( 7, 0, regs );
			_feat7_EBX = regs[1];
			_feat7_ECX = regs[2];
		}

		unsigned int maxExtFeat = pullMaxID( 0x80000000, regs );
		if ( maxExtFeat >= 1 )
		{
			pullID( 0x80000001, regs );
			_feat81_ECX = regs[2];
			_feat81_EDX = regs[3];
		}

		// CPU brand string if available
		if ( maxExtFeat >= 4 )
		{
			pullID( 0x80000002, regs );
			memcpy( _brand, regs.data(), 4*sizeof(unsigned int) );
			pullID( 0x80000003, regs );
			memcpy( _brand + 16, regs.data(), 4*sizeof(unsigned int) );
			pullID( 0x80000004, regs );
			memcpy( _brand + 32, regs.data(), 4*sizeof(unsigned int) );
			_brand[48] = '\0';
		}
		else
			_brand[0] = '\0';

		if ( _amd )
		{
			_prefetch_size = _cache_line_bits;
			if ( maxExtFeat >= 5 )
			{
				pullID( 0x80000005, regs );
				_l1_data_line_size = regs[2] & 0xFF;
				_l1_data_cache = (regs[2] >> 24) * 1024;
				_l1_assoc = (regs[2] >> 16) & 0xFF;
				_l1_instr_line_size = regs[3] & 0xFF;
				_l1_instr_cache = (regs[3] >> 24) * 1024;
			}

			if ( maxExtFeat >= 6 )
			{
				pullID( 0x80000006, regs );
				_l2_data_line_size = regs[2] & 0xFF;
				_l2_data_cache = ( regs[2] >> 16 ) * 1024;
				_l2_assoc = ( regs[2] >> 12 ) & 0xF;
				switch ( _l2_assoc )
				{
					case 0x0:
					case 0x1:
					case 0x2:
					case 0x4:
						break;
					case 0x6: _l2_assoc = 8; break;
					case 0x8: _l2_assoc = 16; break;
					case 0xA: _l2_assoc = 32; break;
					case 0xB: _l2_assoc = 48; break;
					case 0xC: _l2_assoc = 64; break;
					case 0xD: _l2_assoc = 96; break;
					case 0xE: _l2_assoc = 128; break;
					case 0xF: _l2_assoc = 0xFF; break;
					default:
						_l2_assoc = size_t(-1);
						break;
				}
				_l3_data_line_size = regs[3] & 0xFF;
				_l3_data_cache = ( regs[3] >> 18 ) * 512 * 1024;
				_l3_assoc = ( regs[3] >> 12 ) & 0xF;
				switch ( _l3_assoc )
				{
					case 0x0:
					case 0x1:
					case 0x2:
					case 0x4:
						break;
					case 0x6: _l3_assoc = 8; break;
					case 0x8: _l3_assoc = 16; break;
					case 0xA: _l3_assoc = 32; break;
					case 0xB: _l3_assoc = 48; break;
					case 0xC: _l3_assoc = 64; break;
					case 0xD: _l3_assoc = 96; break;
					case 0xE: _l3_assoc = 128; break;
					case 0xF: _l3_assoc = 0xFF; break;
					default:
						_l3_assoc = size_t(-1);
						break;
				}
			}
		}
		else if ( _intel && maxFeat >= 2 )
		{
			pullID( 2, regs );
			bool check4 = false;
			if ( regs[0] == 0 && regs[1] == 0 && regs[2] == 0 && regs[3] == 0 )
			{
				check4 = true;
			}
			else
			{
				int count = regs[0] & 0xFF;
				do
				{
					for ( size_t r = 0; r < 4; ++r )
					{
						if ( (regs[r] & 0x80000000) == 0 )
						{
							for ( size_t i = ((r == 0) ? 1 : 0); i < 3; ++i )
							{
								uint8_t v = ( regs[r] >> (8*i) ) & 0xFF;
								decodeCacheTLB( v, check4 );
							}
						}
					}
					--count;
					if ( count <= 0 )
						break;
					pullID( 2, regs );
				} while ( true );
			}

			if ( check4 )
			{
				_prefetch_size = _cache_line_bits;
				if ( _prefetch_size == 0 )
					_prefetch_size = 64;

				bool next = true;
				for ( unsigned int i = 0; next; ++i )
				{
					pullID( 4, i, regs );
					unsigned int which = ( regs[0] & 0x1F );
					next = which != 0;
					const unsigned int cacheLevel = ( regs[0] >> 5 ) & 7;
					const unsigned int linesize = 1 + ( regs[1] & 0xFFF );
					const unsigned int partitions = 1 + ( (regs[1] >> 12) & 0x3FF );
					const unsigned int ways = 1 + ( ( regs[1] >> 22 ) & 0x3FF );
					const unsigned int sets = 1 + regs[2];
					const unsigned int size = ways * partitions * linesize * sets;
					switch ( which )
					{
						case 1: // data cache
						case 3: // unified cache
							switch ( cacheLevel )
							{
								case 1:
									_l1_data_cache = size;
									_l1_data_line_size = linesize;
									_l1_assoc = ways;
									break;
								case 2:
									_l2_data_cache = size;
									_l2_data_line_size = linesize;
									_l2_assoc = ways;
									
									break;
								case 3:
									_l3_data_cache = size;
									_l3_data_line_size = linesize;
									_l3_assoc = ways;
									break;
								default:
									break;
							}
							break;
						case 2: // instr cache
							if ( cacheLevel == 1 )
							{
								_l1_instr_cache = size;
								_l1_instr_line_size = linesize;
							}
							break;

						case 0:
						default:
							break;
					}
				}
			}
		}
#endif // IS_INTEL_ISH
	}

#ifdef IS_INTEL_ISH
	void decodeCacheTLB( uint8_t v, bool &check4 )
	{
		switch ( v )
		{
			case 0x00: // null
				break;
			case 0x01: // TLB - instr TLB 4KB pages, 4-way set associative, 32 entries
			case 0x02: // TLB - instr TLB 4MB pages, fully assoc, 2 entries
			case 0x03: // Data TLB - 4KB pages, 4-way set assoc, 64 entries
			case 0x04: // Data TLB - 4MB pages, 4-way set assoc, 8 entries
			case 0x05: // Data TLB1 - 4MB pages, 4-way set assoc, 32 entries
				break;
			case 0x06: // cache L1 instr, 8KB, 4-way set assoc, 32 byte line size
				_l1_instr_cache = 8 * 1024;
				_l1_instr_line_size = 32;
				_l1_instr_assoc = 4;
				break;
			case 0x08: // cache L1 instr, 16KB, 4-way set assoc, 32 byte line size
				_l1_instr_cache = 16 * 1024;
				_l1_instr_line_size = 32;
				_l1_instr_assoc = 4;
				break;
			case 0x09: // cache L1 instr, 32KB, 4-way set assoc, 64 byte line size
				_l1_instr_cache = 32 * 1024;
				_l1_instr_line_size = 64;
				_l1_instr_assoc = 4;
				break;
			case 0x0A: // cache L1 data, 8KB, 2-way set assoc, 32 byte line size
				_l1_data_cache = 8 * 1024;
				_l1_data_line_size = 32;
				_l1_assoc = 2;
				break;
			case 0x0B: // TLB instr 4MB page, 4-way, 4 entries
			case 0x0C: // L1 data: 16KB, 4-way, 32 byte
				_l1_data_cache = 16 * 1024;
				_l1_data_line_size = 32;
				_l1_assoc = 4;
				break;
			case 0x0D: // L1 data: 16KB, 4-way, 64 byte
				_l1_data_cache = 16 * 1024;
				_l1_data_line_size = 64;
				_l1_assoc = 4;
				break;
			case 0x0E: // L1 data: 24KB, 6-way, 64 byte
				_l1_data_cache = 24 * 1024;
				_l1_data_line_size = 64;
				_l1_assoc = 6;
				break;
			case 0x1D: // L2 cache: 128KB, 2-way, 64 byte
				_l2_data_cache = 128 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 2;
				break;
			case 0x21: // L2 cache: 256KB, 8-way, 64 byte
				_l2_data_cache = 256 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0x22: // L3 cache: 512KB, 4-way, 64 byte, 2 lines per sector
				_l3_data_cache = 512 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 4;
				break;
			case 0x23: // L3 cache: 1MB, 4-way, 64 byte, 2 lines per sector
				_l3_data_cache = 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 8;
				break;
			case 0x24: // L2 1MB, 16-way, 64 byte
				_l2_data_cache = 1024 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 16;
				break;
			case 0x25: // L3 cache: 2MB, 8-way, 64 byte, 2 lines per sector
				_l3_data_cache = 2 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 8;
				break;
			case 0x29: // L3 cache: 4MB, 8-way, 64 byte, 2 lines per sector
				_l3_data_cache = 4 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 8;
				break;
			case 0x2C: // L1 data cache: 32 KB, 8-way, 64 byte
				_l1_data_cache = 32 * 1024;
				_l1_data_line_size = 64;
				_l1_assoc = 8;
				break;
			case 0x30: // L1 instr cache: 32 KB, 8-way, 64 byte
				_l1_instr_cache = 32 * 1024;
				_l1_instr_line_size = 64;
				_l1_instr_assoc = 8;
				break;
			case 0x40:
				// no L2 or if valid L2, no L3
				// anything to flag here?
				break;
			case 0x41: // L2 data cache: 128 KB, 4-way, 32 byte
				_l2_data_cache = 128 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 4;
				break;
			case 0x42: // L2 data cache: 256 KB, 4-way, 32 byte
				_l2_data_cache = 256 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 4;
				break;
			case 0x43: // L2 data cache: 512 KB, 4-way, 32 byte
				_l2_data_cache = 512 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 4;
				break;
			case 0x44: // L2 data cache: 1MB, 4-way, 32 byte
				_l2_data_cache = 1024 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 4;
				break;
			case 0x45: // L2 data cache: 2MB, 4-way, 32 byte
				_l2_data_cache = 2 * 1024 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 4;
				break;
			case 0x46: // L3 data cache: 4MB, 4-way, 64 byte
				_l3_data_cache = 4 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 4;
				break;
			case 0x47: // L3 data cache: 8MB, 8-way, 64 byte
				_l3_data_cache = 8 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 8;
				break;
			case 0x48: // L2 data cache: 3MB, 12-way, 64 byte
				_l2_data_cache = 3 * 1024 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 12;
				break;
			case 0x49:
				// intel zeon processor MP, L3 4MB, 16-way, 64 byte
				// L2 4MB, 16-way, 64byte otherwise
				if ( _proc_family == 0xf && _proc_model == 0x6 )
				{
					_l3_data_cache = 4 * 1024 * 1024;
					_l3_data_line_size = 64;
					_l3_assoc = 16;
				}
				else
				{
					_l2_data_cache = 4 * 1024 * 1024;
					_l2_data_line_size = 64;
					_l2_assoc = 16;
				}
				break;
			case 0x4A: // L3 6MB, 12-way, 64 byte
				_l3_data_cache = 6 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 12;
				break;
			case 0x4B: // L3 8MB, 16-way, 64 byte
				_l3_data_cache = 8 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 16;
				break;
			case 0x4C: // L3 12MB, 12-way, 64 byte
				_l3_data_cache = 12 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 12;
				break;
			case 0x4D: // L3 16MB, 16-way, 64 byte
				_l3_data_cache = 16 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 16;
				break;
			case 0x4E: // L2 6MB, 24-way, 64 byte
				_l2_data_cache = 6 * 1024 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 24;
				break;
			case 0x4F: // Instr TLB 4KB pages, 32 entries
			case 0x50: // Instr TLB 4KB and 2MB or 4MB pages, 64 entries
			case 0x51: // Instr TLB 4KB and 2MB or 4MB pages, 128 entries
			case 0x52: // Instr TLB 4KB and 2MB or 4MB pages, 256 entries
			case 0x55: // Instr TLB 2MB or 4MB pages, fully assoc, 7 entries
			case 0x56: // Data TLB0: 4MB pages, 4-way, 16 entries
			case 0x57: // Data TLB0: 4KB pages, 4-way, 16 entries
			case 0x59: // Data TLB0: 4KB pages, fully assoc, 16 entries
			case 0x5A: // Data TLB0: 2MB or 4MB pages, 4-way assoc, 32 entries
			case 0x5B: // Data TLB: 4KB and 4MB pages, 64 entries
			case 0x5C: // Data TLB: 4KB and 4MB pages, 128 entries
			case 0x5D: // Data TLB: 4KB and 4MB pages, 256 entries
				break;
			case 0x60: // L1 data: 16KB, 8-way, 64 byte
				_l1_data_cache = 16 * 1024;
				_l1_data_line_size = 64;
				_l1_assoc = 8;
				break;
			case 0x61: // Instr TLB: 4KB pages, fully assoc, 48 entries
			case 0x63: // Data TLB: 2MB or 4MB pages, 4-way, 32 entries, separate array w/ 1GB pages, 4-way, 4 entries
			case 0x64: // Data TLB: 4KB pages, 4-way assoc, 512 entries
			case 0x66: // L1 data: 8KB, 4-way, 64 byte
				_l1_data_cache = 8 * 1024;
				_l1_data_line_size = 64;
				_l1_assoc = 4;
				break;
			case 0x67: // L1 data: 16KB, 4-way, 64 byte
				_l1_data_cache = 16 * 1024;
				_l1_data_line_size = 64;
				_l1_assoc = 4;
				break;
			case 0x68: // L1 data: 32KB, 4-way, 64 byte
				_l1_data_cache = 32 * 1024;
				_l1_data_line_size = 64;
				_l1_assoc = 4;
				break;
			case 0x6A: // uTLB: 4KB pages, 8-way, 64 entries
			case 0x6B: // DTLB: 4KB pages, 8-way, 256 entries
			case 0x6C: // DTLB: 2M/4M pages, 8-way, 128 entries
			case 0x6D: // DTLB: 1GB pages, fully assoc, 16 entries
			case 0x70: // Trace cache: 12 K-uop, 8-way assoc
			case 0x71: // Trace cache: 16 K-uop, 8-way assoc
			case 0x72: // Trace cache: 32 K-uop, 8-way assoc
			case 0x76: // Instr TLB: 2M/4M pages, fully assoc, 8 entries
			case 0x78: // L2 cache: 1MB, 4-way, 64 byte
				_l2_data_cache = 1024 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 4;
				break;
			case 0x79: // L2 cache: 128KB, 8-way, 64 byte, 2 lines per sector
				_l2_data_cache = 128 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0x7A: // L2 cache: 256KB, 8-way, 64 byte, 2 lines per sector
				_l2_data_cache = 256 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0x7B: // L2 cache: 512KB, 8-way, 64 byte, 2 lines per sector
				_l2_data_cache = 512 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0x7C: // L2 cache: 1MB, 8-way, 64 byte, 2 lines per sector
				_l2_data_cache = 1024 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0x7D: // L2 cache: 2MB, 8-way, 64 byte
				_l2_data_cache = 2 * 1024 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0x7F: // L2 cache: 512KB, 2-way, 64 byte
				_l2_data_cache = 512 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 2;
				break;
			case 0x80: // L2 cache: 512KB, 8-way, 64 byte
				_l2_data_cache = 512 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0x82: // L2 cache: 256KB, 8-way, 32 byte
				_l2_data_cache = 256 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 8;
				break;
			case 0x83: // L2 cache: 512KB, 8-way, 32 byte
				_l2_data_cache = 512 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 8;
				break;
			case 0x84: // L2 cache: 1MB, 8-way, 32 byte
				_l2_data_cache = 1024 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 8;
				break;
			case 0x85: // L2 cache: 2MB, 8-way, 32 byte
				_l2_data_cache = 2 * 1024 * 1024;
				_l2_data_line_size = 32;
				_l2_assoc = 8;
				break;
			case 0x86: // L2 cache: 512KB, 4-way, 64 byte
				_l2_data_cache = 512 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 4;
				break;
			case 0x87: // L2 cache: 1MB, 8-way, 64 byte
				_l2_data_cache = 1024 * 1024;
				_l2_data_line_size = 64;
				_l2_assoc = 8;
				break;
			case 0xA0: // DTLB 4K pages, fully assoc, 32 entries
			case 0xB0: // Instr TLB: 4K pages, 4-way assoc, 128 entries
			case 0xB1: // Instr TLB: 2M pages, 4-way assoc, 8 entries or 4M pages, 4-way, 4 entries
			case 0xB2: // Instr TLB: 4K pages, 4-way assoc, 64 entries
			case 0xB3: // Data TLB1: 4K pages, 4-way assoc, 128 entries
			case 0xB4: // Data TLB1: 4K pages, 4-way assoc, 256 entries
			case 0xB5: // Instr TLB: 4K pages, 8-way assoc, 64 entries
			case 0xB6: // Instr TLB: 4K pages, 8-way assoc, 128 entries
			case 0xBA: // Data TLB1: 4KB pages, 4-way assoc, 64 entries
			case 0xC0: // Data TLB: 4KB and 4MB pages, 4-way assoc, 8 entries
			case 0xC1: // shared L2 TLB: 4KB/2MB pages, 8-way assoc, 1024 entries
			case 0xC2: // DTLB: 4KB/2MB pages, 4-way assoc, 16 entries
			case 0xC3: // shared L2 TLB: 4KB / 2MB pages, 6-way assoc, 1536 entries + 1GB pages, 4-way, 16 entries
			case 0xC4: // DTLB: 2M/4MB pages, 4-way assoc, 32 entries
			case 0xCA: // shared L2 TLB: 4KB pages, 4-way assoc, 512 entries
				break;
			case 0xD0: // L3 cache: 512K, 4-way, 64
				_l3_data_cache = 512 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 4;
				break;
			case 0xD1: // L3 cache: 1MB, 4-way, 64
				_l3_data_cache = 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 4;
				break;
			case 0xD2: // L3 cache: 2MB, 4-way, 64
				_l3_data_cache = 2 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 4;
				break;
			case 0xD6: // L3 cache: 1MB, 8-way, 64
				_l3_data_cache = 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 8;
				break;
			case 0xD7: // L3 cache: 2MB, 8-way, 64
				_l3_data_cache = 2 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 8;
				break;
			case 0xD8: // L3 cache: 4MB, 8-way, 64
				_l3_data_cache = 4 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 8;
				break;
			case 0xDC: // L3 cache: 1.5MB, 12-way, 64
				_l3_data_cache = 3 * 512 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 12;
				break;
			case 0xDD: // L3 cache: 3MB, 12-way, 64
				_l3_data_cache = 3 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 12;
				break;
			case 0xDE: // L3 cache: 6MB, 12-way, 64
				_l3_data_cache = 6 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 12;
				break;
			case 0xE2: // L3 cache: 2MB, 16-way, 64
				_l3_data_cache = 2 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 16;
				break;
			case 0xE3: // L3 cache: 4MB, 16-way, 64
				_l3_data_cache = 4 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 16;
				break;
			case 0xE4: // L3 cache: 8MB, 16-way, 64
				_l3_data_cache = 8 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 16;
				break;
			case 0xEA: // L3 cache: 12MB, 24-way, 64
				_l3_data_cache = 12 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 24;
				break;
			case 0xEB: // L3 cache: 18MB, 24-way, 64
				_l3_data_cache = 18 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 24;
				break;
			case 0xEC: // L3 cache: 24MB, 24-way, 64
				_l3_data_cache = 24 * 1024 * 1024;
				_l3_data_line_size = 64;
				_l3_assoc = 24;
				break;
			case 0xF0:
				_prefetch_size = 64;
				break;
			case 0xF1:
				_prefetch_size = 128;
				break;
			case 0xFF:
				// we have to use CPUID(4) to find out
				check4 = true;
				break;
			default:
				break;
		}
	}

	char _vendor[16];
	char _brand[64];
	size_t _proc_family;
	size_t _proc_model;
	bool _intel = false;
	bool _amd = false;

	// we pull the other registers, but do we care?
	std::bitset<32> _feat1_ECX;
	std::bitset<32> _feat1_EDX;
	std::bitset<32> _feat7_EBX;
	std::bitset<32> _feat7_ECX;
	std::bitset<32> _feat81_ECX;
	std::bitset<32> _feat81_EDX;

	size_t _cache_line_bits = 0;
	size_t _l1_instr_cache = 0;
	size_t _l1_instr_line_size = 0;
	size_t _l1_instr_assoc = 0;
	size_t _l1_data_cache = 0;
	size_t _l1_data_line_size = 0;
	size_t _l1_assoc = 0;
	size_t _l2_data_cache = 0;
	size_t _l2_data_line_size = 0;
	size_t _l2_assoc = 0;
	size_t _l3_data_cache = 0;
	size_t _l3_data_line_size = 0;
	size_t _l3_assoc = 0;
	size_t _prefetch_size = 32;
#endif
};

static CPUFeatureStore theFeatures;
}


////////////////////////////////////////


namespace base
{

const char *cpu::vendor( void ) 
{ return theFeatures._vendor; }

const char *cpu::brand( void )
{ return theFeatures._brand; }

void
cpu::output( std::ostream &os )
{
	if ( theFeatures._brand[0] != '\0' )
		os << theFeatures._brand << " (" << theFeatures._vendor << ")";
	else
		os << theFeatures._vendor;

#if IS_INTEL_ISH
	os << "\n  features [";
	if ( has_MMX() ) os << " mmx";
	if ( has_MMX_EXT() ) os << " mmxext";
	if ( has_SSE() ) os << " sse";
	if ( has_SSE2() ) os << " sse2";
	if ( has_SSE3() ) os << " sse3";
	if ( has_SSSE3() ) os << " ssse3";
	if ( has_SSE41() ) os << " sse41";
	if ( has_SSE42() ) os << " sse42";
	if ( has_SSE4a() ) os << " sse4a";
	if ( has_3DNOW() ) os << " 3dnow";
	if ( has_3DNOW_EXT() ) os << " 3dnowext";
	if ( has_AVX() ) os << " avx";
	if ( has_AVX2() ) os << " avx2";
	if ( has_AVX512F() ) os << " avx512f";
	if ( has_AVX512DQ() ) os << " avx512dq";
	if ( has_AVX512PF() ) os << " avx512pf";
	if ( has_AVX512ER() ) os << " avx512er";
	if ( has_AVX512CD() ) os << " avx512cd";
	if ( has_AVX512BW() ) os << " avx512bw";
	if ( has_AVX512VL() ) os << " avx512vl";
	if ( has_AVX512BMI() ) os << " avx512bmi";
	if ( has_FMA() ) os << " fma";
	if ( has_FMA4() ) os << " fma4";
	if ( has_PCLMULQDQ() ) os << " pclmuldq";
	if ( has_MONITOR() ) os << " monitor";
	if ( has_CMPXCHG16B() ) os << " cmpxchg16b";
	if ( has_PDCM() ) os << " pdcm";
	if ( has_DCA() ) os << " dca";
	if ( has_MOVBE() ) os << " movbe";
	if ( has_POPCNT() ) os << " popcnt";
	if ( has_AES() ) os << " aes";
	if ( has_XSAVE() ) os << " xsave";
	if ( has_OSXSAVE() ) os << " osxsave";
	if ( has_F16C() ) os << " f16c";
	if ( has_RDRAND() ) os << " rdrand";
	if ( has_MSR() ) os << " msr";
	if ( has_CX8() ) os << " cx8";
	if ( has_SEP() ) os << " sep";
	if ( has_CMOV() ) os << " cmov";
	if ( has_CLFSH() ) os << " clfsh";
	if ( has_FXSR() ) os << " fxsr";
	if ( has_FSGSBASE() ) os << " fsgsbase";
	if ( has_BMI1() ) os << " bmi1";
	if ( has_HLE() ) os << " hle";
	if ( has_BMI2() ) os << " bmi2";
	if ( has_ERMS() ) os << " erms";
	if ( has_INVPCID() ) os << " invpcid";
	if ( has_RTM() ) os << " rtm";
	if ( has_RDSEED() ) os << " rdseed";
	if ( has_ADX() ) os << " adx";
	if ( has_SHA() ) os << " sha";
	if ( has_PREFETCHWT1() ) os << " prefetchwt1";
	if ( has_LAHF() ) os << " lahf";
	if ( has_LZCNT() ) os << " lzcnt";
	if ( has_ABM() ) os << " abm";
	if ( has_XOP() ) os << " xop";
	if ( has_TBM() ) os << " tbm";
	if ( has_SYSCALL() ) os << " syscall";
	if ( has_RDTSCP() ) os << " rdtscp";
	os << " ]\n  cache_line_bits "
	<< cache_line_bits() << " prefetch " << prefetch() << " bytes"
	<< "\n  L1 instr: [ " << l1_instruction_cache() << " / " << l1_instruction_line_size() << " byte lines / " << l1_instruction_associativity() << "-way assoc ]"
	<< "\n  L1 data: [ " << l1_data_cache() << " / " << l1_data_line_size() << " byte lines / " << l1_associativity() << "-way assoc ]"
	<< "\n  L2 data: [ " << l2_data_cache() << " / " << l2_data_line_size() << " byte lines / " << l2_associativity() << "-way assoc ]"
	<< "\n  L3 data: [ " << l3_data_cache() << " / " << l3_data_line_size() << " byte lines / " << l3_associativity() << "-way assoc ]";
#endif
}

////////////////////////////////////////

bool cpu::is_intel( void ) { return theFeatures._intel; }
bool cpu::is_amd( void ) { return theFeatures._amd; }

size_t cpu::cache_line_bits( void ) { return theFeatures._cache_line_bits; }

size_t cpu::l1_instruction_cache( void ) { return theFeatures._l1_instr_cache; }
size_t cpu::l1_instruction_line_size( void ) { return theFeatures._l1_instr_line_size; }
size_t cpu::l1_instruction_associativity( void ) { return theFeatures._l1_instr_assoc; }

size_t cpu::l1_data_cache( void ) { return theFeatures._l1_data_cache; }
size_t cpu::l1_data_line_size( void ) { return theFeatures._l1_data_line_size; }
size_t cpu::l1_associativity( void ) { return theFeatures._l1_assoc; }
size_t cpu::l2_data_cache( void ) { return theFeatures._l2_data_cache; }
size_t cpu::l2_data_line_size( void ) { return theFeatures._l2_data_line_size; }
size_t cpu::l2_associativity( void ) { return theFeatures._l2_assoc; }
size_t cpu::l3_data_cache( void ) { return theFeatures._l3_data_cache; }
size_t cpu::l3_data_line_size( void ) { return theFeatures._l3_data_line_size; }
size_t cpu::l3_associativity( void ) { return theFeatures._l3_assoc; }
size_t cpu::prefetch( void ) { return theFeatures._prefetch_size; }

////////////////////////////////////////

bool cpu::has_simd( void ) { return highest_simd() != simd_feature::NONE; }

#if IS_INTEL_ISH
// intel added fast unaligned loads with nehalem, which added POPCNT, in case the misaligned SSE flag isn't set
bool cpu::fast_unaligned_load( void ) { return has_misaligned_sse() || ( theFeatures._intel && has_POPCNT() ); }
bool cpu::has_misaligned_sse( void ) { return theFeatures._feat81_ECX[7]; }
bool cpu::has_MMX( void ) { return theFeatures._feat1_EDX[23]; }
bool cpu::has_MMX_EXT( void ) { return theFeatures._amd && theFeatures._feat81_EDX[22]; }
bool cpu::has_SSE( void ) { return theFeatures._feat1_EDX[25]; }
bool cpu::has_SSE2( void ) { return theFeatures._feat1_EDX[26]; }
bool cpu::has_SSE3( void ) { return theFeatures._feat1_ECX[0]; }
bool cpu::has_SSSE3( void ) { return theFeatures._feat1_ECX[9]; }
bool cpu::has_SSE41( void ) { return theFeatures._feat1_ECX[19]; }
bool cpu::has_SSE42( void ) { return theFeatures._feat1_ECX[20]; }
bool cpu::has_SSE4a( void ) { return theFeatures._amd && theFeatures._feat81_ECX[6]; }
bool cpu::has_3DNOW( void ) { return theFeatures._amd && theFeatures._feat81_EDX[31]; }
bool cpu::has_3DNOW_EXT( void ) { return theFeatures._amd && theFeatures._feat81_EDX[30]; }
bool cpu::has_AVX( void ) { return theFeatures._feat1_ECX[28]; }
bool cpu::has_AVX2( void ) { return theFeatures._feat7_EBX[5]; }
bool cpu::has_AVX512F( void ) { return theFeatures._feat7_EBX[16]; }
bool cpu::has_AVX512DQ( void ) { return theFeatures._feat7_EBX[17]; }
bool cpu::has_AVX512PF( void ) { return theFeatures._feat7_EBX[26]; }
bool cpu::has_AVX512ER( void ) { return theFeatures._feat7_EBX[27]; }
bool cpu::has_AVX512CD( void ) { return theFeatures._feat7_EBX[28]; }
bool cpu::has_AVX512BW( void ) { return theFeatures._feat7_EBX[30]; }
bool cpu::has_AVX512VL( void ) { return theFeatures._feat7_EBX[31]; }
bool cpu::has_AVX512BMI( void ) { return theFeatures._feat7_ECX[1]; }

bool cpu::has_FMA( void ) { return theFeatures._feat1_ECX[12]; }
bool cpu::has_FMA4( void ) { return theFeatures._feat81_ECX[16]; }

bool cpu::has_PCLMULQDQ( void ) { return theFeatures._feat1_ECX[1]; }
bool cpu::has_MONITOR( void ) { return theFeatures._feat1_ECX[3]; }
bool cpu::has_CMPXCHG16B( void ) { return theFeatures._feat1_ECX[13]; }
bool cpu::has_PDCM( void ) { return theFeatures._feat1_ECX[15]; }
bool cpu::has_DCA( void ) { return theFeatures._feat1_ECX[18]; }
bool cpu::has_MOVBE( void ) { return theFeatures._feat1_ECX[22]; }
bool cpu::has_POPCNT( void ) { return theFeatures._feat1_ECX[23]; }
bool cpu::has_AES( void ) { return theFeatures._feat1_ECX[25]; }
bool cpu::has_XSAVE( void ) { return theFeatures._feat1_ECX[26]; }
bool cpu::has_OSXSAVE( void ) { return theFeatures._feat1_ECX[27]; }
bool cpu::has_F16C( void ) { return theFeatures._feat1_ECX[29]; }
bool cpu::has_RDRAND( void ) { return theFeatures._feat1_ECX[30]; }

bool cpu::has_MSR( void ) { return theFeatures._feat1_EDX[5]; }
bool cpu::has_CX8( void ) { return theFeatures._feat1_EDX[8]; }
bool cpu::has_SEP( void ) { return theFeatures._feat1_EDX[11]; }
bool cpu::has_CMOV( void ) { return theFeatures._feat1_EDX[15]; }
bool cpu::has_CLFSH( void ) { return theFeatures._feat1_EDX[19]; }
bool cpu::has_FXSR( void ) { return theFeatures._feat1_EDX[24]; }

bool cpu::has_FSGSBASE( void ) { return theFeatures._feat7_EBX[0]; }
bool cpu::has_BMI1( void ) { return theFeatures._feat7_EBX[3]; }
bool cpu::has_HLE( void ) { return theFeatures._intel && theFeatures._feat7_EBX[4]; }
bool cpu::has_BMI2( void ) { return theFeatures._feat7_EBX[8]; }
bool cpu::has_ERMS( void ) { return theFeatures._feat7_EBX[9]; }
bool cpu::has_INVPCID( void ) { return theFeatures._feat7_EBX[10]; }
bool cpu::has_RTM( void ) { return theFeatures._intel && theFeatures._feat7_EBX[11]; }
bool cpu::has_RDSEED( void ) { return theFeatures._feat7_EBX[18]; }
bool cpu::has_ADX( void ) { return theFeatures._feat7_EBX[19]; }
bool cpu::has_SHA( void ) { return theFeatures._feat7_EBX[29]; }

bool cpu::has_AMD_PREFETCHW( void ) { return theFeatures._feat81_ECX[8]; }
bool cpu::has_PREFETCHWT1( void ) { return theFeatures._feat7_ECX[0]; }

bool cpu::has_LAHF( void ) { return theFeatures._feat81_ECX[0]; }
bool cpu::has_LZCNT( void ) { return theFeatures._intel && theFeatures._feat81_ECX[5]; }
bool cpu::has_ABM( void ) { return theFeatures._amd && theFeatures._feat81_ECX[5]; }
bool cpu::has_XOP( void ) { return theFeatures._amd && theFeatures._feat81_ECX[11]; }
bool cpu::has_TBM( void ) { return theFeatures._amd && theFeatures._feat81_ECX[21]; }

bool cpu::has_SYSCALL( void ) { return theFeatures._feat81_EDX[11]; }
bool cpu::has_RDTSCP( void ) { return theFeatures._feat81_EDX[27]; }
#else
bool cpu::fast_unaligned_load( void ) { return false; }
bool cpu::has_misaligned_sse( void ) { return false; }
bool cpu::has_MMX( void ) { return false; }
bool cpu::has_MMX_EXT( void ) { return false; }
bool cpu::has_SSE( void ) { return false; }
bool cpu::has_SSE2( void ) { return false; }
bool cpu::has_SSE3( void ) { return false; }
bool cpu::has_SSSE3( void ) { return false; }
bool cpu::has_SSE41( void ) { return false; }
bool cpu::has_SSE42( void ) { return false; }
bool cpu::has_SSE4a( void ) { return false; }
bool cpu::has_3DNOW( void ) { return false; }
bool cpu::has_3DNOW_EXT( void ) { return false; }
bool cpu::has_AVX( void ) { return false; }
bool cpu::has_AVX2( void ) { return false; }
bool cpu::has_AVX512F( void ) { return false; }
bool cpu::has_AVX512DQ( void ) { return false; }
bool cpu::has_AVX512PF( void ) { return false; }
bool cpu::has_AVX512ER( void ) { return false; }
bool cpu::has_AVX512CD( void ) { return false; }
bool cpu::has_AVX512BW( void ) { return false; }
bool cpu::has_AVX512VL( void ) { return false; }
bool cpu::has_AVX512BMI( void ) { return false; }
bool cpu::has_FMA( void ) { return false; }
bool cpu::has_FMA4( void ) { return false; }
bool cpu::has_PCLMULQDQ( void ) { return false; }
bool cpu::has_MONITOR( void ) { return false; }
bool cpu::has_CMPXCHG16B( void ) { return false; }
bool cpu::has_PDCM( void ) { return false; }
bool cpu::has_DCA( void ) { return false; }
bool cpu::has_MOVBE( void ) { return false; }
bool cpu::has_POPCNT( void ) { return false; }
bool cpu::has_AES( void ) { return false; }
bool cpu::has_XSAVE( void ) { return false; }
bool cpu::has_OSXSAVE( void ) { return false; }
bool cpu::has_F16C( void ) { return false; }
bool cpu::has_RDRAND( void ) { return false; }
bool cpu::has_MSR( void ) { return false; }
bool cpu::has_CX8( void ) { return false; }
bool cpu::has_SEP( void ) { return false; }
bool cpu::has_CMOV( void ) { return false; }
bool cpu::has_CLFSH( void ) { return false; }
bool cpu::has_FXSR( void ) { return false; }
bool cpu::has_FSGSBASE( void ) { return false; }
bool cpu::has_BMI1( void ) { return false; }
bool cpu::has_HLE( void ) { return false; }
bool cpu::has_BMI2( void ) { return false; }
bool cpu::has_ERMS( void ) { return false; }
bool cpu::has_INVPCID( void ) { return false; }
bool cpu::has_RTM( void ) { return false; }
bool cpu::has_RDSEED( void ) { return false; }
bool cpu::has_ADX( void ) { return false; }
bool cpu::has_SHA( void ) { return false; }
bool cpu::has_AMD_PREFETCHW( void ) { return false; }
bool cpu::has_PREFETCHWT1( void ) { return false; }
bool cpu::has_LAHF( void ) { return false; }
bool cpu::has_LZCNT( void ) { return false; }
bool cpu::has_ABM( void ) { return false; }
bool cpu::has_XOP( void ) { return false; }
bool cpu::has_TBM( void ) { return false; }
bool cpu::has_SYSCALL( void ) { return false; }
bool cpu::has_RDTSCP( void ) { return false; }
#endif

cpu::simd_feature cpu::highest_simd( void )
{
#if IS_INTEL_ISH
	if ( has_AVX512F() )
		return simd_feature::AVX512F;
	if ( has_AVX2() )
		return simd_feature::AVX2;
	if ( has_AVX() )
		return simd_feature::AVX;
	if ( has_SSE42() )
		return simd_feature::SSE42;
	if ( has_SSE41() )
		return simd_feature::SSE41;
	if ( has_SSE3() )
		return simd_feature::SSE3;
	if ( has_SSE2() )
		return simd_feature::SSE2;
	if ( has_SSE() )
		return simd_feature::SSE;
#elif IS_ARM
	if ( has_NEON() )
		return simd_feature::NEON;
#endif
	return simd_feature::NONE;
}

/// @}

bool cpu::has_NEON( void ) {
#if defined(IS_ARM)
	return XXXXX;
#else
	return false;
#endif
}


////////////////////////////////////////

} // base



