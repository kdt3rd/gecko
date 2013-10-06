
#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <memory>

#include "hash256.h"

////////////////////////////////////////

class data_reader
{
public:
	data_reader( const std::string &fn );
	data_reader( data_reader &&other );

	~data_reader( void );

	char next_type( void );

	// Read null
	void read_null( void );

	// Read true/false
	void read( bool &v );

	// Read a character
	void read( char &c );

	// Read 8-bit byte
	void read( int8_t &v );
	void read( uint8_t &v );

	// Read 16-bit integer
	void read( int16_t &v );
	void read( uint16_t &v );

	// Read 32-bit integer
	void read( int32_t &v );
	void read( uint32_t &v );

	// Read 64-bit integer
	void read( int64_t &v );
	void read( uint64_t &v );

	// Read 32-bit floating point number
	void read( float &v );

	// Read 64-bit floating point number
	void read( double &v );

	// Read string
	void read( std::string &v );

	// Read no-op
	void read_noop();

	// Begin reading an object
	size_t read_object( void );

	// Begin reading an array
	size_t read_array( void );

	// End an object or array of unknown size
	void read_end( void );

private:
	uint32_t read32( void );

	std::shared_ptr<std::ifstream> _file;
};

////////////////////////////////////////

