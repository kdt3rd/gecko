
#pragma once

namespace base
{

////////////////////////////////////////

inline uint16_t byteswap( uint16_t c )
{
	union
	{
		uint16_t n;
		uint8_t c[2];
	} bytes;
	bytes.n = c;
	std::swap( bytes.c[0], bytes.c[1] );
	return bytes.n;
}

////////////////////////////////////////

inline uint32_t byteswap( uint32_t c )
{
	union
	{
		uint32_t n;
		uint8_t c[4];
	} bytes;
	bytes.n = c;
	std::swap( bytes.c[0], bytes.c[3] );
	std::swap( bytes.c[1], bytes.c[2] );
	return bytes.n;
}

////////////////////////////////////////

inline uint64_t byteswap( uint64_t c )
{
	union
	{
		uint64_t n;
		uint8_t c[8];
	} bytes;
	bytes.n = c;
	std::swap( bytes.c[0], bytes.c[7] );
	std::swap( bytes.c[1], bytes.c[6] );
	std::swap( bytes.c[2], bytes.c[5] );
	std::swap( bytes.c[3], bytes.c[4] );
	return bytes.n;
}

////////////////////////////////////////


}
