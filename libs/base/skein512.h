//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <array>
#include <string>

namespace base
{

////////////////////////////////////////

class skein512
{
public:
	typedef std::array<uint8_t,64> hash;

	skein512( void );

	inline void operator()( const void *message, size_t length ) { update( message, length ); }
	inline void operator()( bool message ) { update( message ? "T" : "F", 1 ); }
	inline void operator()( char message ) { update( &message, sizeof( message ) ); }
	inline void operator()( uint8_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( uint16_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( uint32_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( uint64_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( int8_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( int16_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( int32_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( int64_t message ) { update( &message, sizeof( message ) ); }
	inline void operator()( float message ) { update( &message, sizeof( message ) ); }
	inline void operator()( double message ) { update( &message, sizeof( message ) ); }
	inline void operator()( const std::string &message ) { update( message.c_str(), message.size() ); }
	inline void operator()( const std::u32string &message ) { update( message.c_str(), message.size() * sizeof(char32_t) ); }

	hash get_hash( void );
	std::string hash_string( void );

private:
	void update( const void *msg, size_t length );
	void process( const uint8_t *blkPtr, size_t blkCnt, size_t byteCntAdd );
	void start_new_type( uint64_t block_type );
	void finalize( hash &h );

    size_t  _block_size; // current byte count in buffer b[] */
    uint64_t  _T[2]; // tweak words: T[0]=byte cnt, T[1]=flags */
    uint64_t  _X[8]; // chaining variables */
	union
	{
		uint8_t  _block[64]; // partial block buffer (8-byte aligned) */
		uint64_t _block64[8];
	};
	hash _hash;
	bool _finalized = false;
};

////////////////////////////////////////

}

