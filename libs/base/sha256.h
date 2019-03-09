// Copyright (c) 2015-2016 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <array>
#include <string>

namespace base
{

////////////////////////////////////////

/// @brief SHA256 encryption.
class sha256
{
public:
	typedef std::array<uint8_t,32> hash;

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
	void update( const void *message, size_t length );
	void transform( const void *message, size_t block_nb );
	void finalize( void );

	uint32_t _hash[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
	uint8_t _block[128] = {};
	size_t _tot_len = 0;
	size_t _len = 0;
	bool _finalized = false;
};

////////////////////////////////////////

}
