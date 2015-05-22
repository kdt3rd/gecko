
#pragma once

#include <array>
#include <string>

namespace base
{

////////////////////////////////////////

class sha160
{
public:
	typedef std::array<uint8_t,20> hash;

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
	inline void operator()( const std::u32string &message ) { update( message.c_str(), message.size() * sizeof( char32_t ) ); }

	hash get_hash( void );
	std::string hash_string( void );

private:
	void update( const void *msg, size_t bytes );
	void finalize( void );

	void transform( void );

	bool _finalized = false;
	uint32_t _digest[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };
	uint32_t _block[16] = {};
	size_t _len = 0;
	uint64_t _transforms = 0;
};

////////////////////////////////////////

}


