
#pragma once

#include <stdint.h>

////////////////////////////////////////

namespace data_type
{
	const char null = 'Z';
	const char yes = 'T';
	const char no = 'F';
	const char byte = 'C';
	const char int8 = 'b';
	const char int16 = 'w';
	const char int32 = 'l';
	const char int64 = 'q';
	const char uint8 = 'B';
	const char uint16 = 'W';
	const char uint32 = 'L';
	const char uint64 = 'Q';
	const char float32 = 'f';
	const char float64 = 'd';
	const char string8 = 's';
	const char string32 = 'S';
	const char object8 = 'o';
	const char object32 = 'O';
	const char array8 = 'a';
	const char array32 = 'A';
	const char noop = 'N';
	const char end = 'E';
};

constexpr uint32_t tag( const char t[4] )
{
	return (uint32_t(uint8_t(t[0])) << 24) + (uint32_t(uint8_t(t[1])) << 16) + (uint32_t(uint8_t(t[2])) << 8) + uint32_t(uint8_t(t[3]));
}

////////////////////////////////////////

