
#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <memory>

#include <botan/sha2_32.h>

#include "hash256.h"

////////////////////////////////////////

class data_writer
{
public:
	data_writer( std::string filename );
	data_writer( data_writer &&other );

	~data_writer( void );

	// Write null
	void write_null( void );

	// Write true/false
	void write( bool v );

	// Write a character
	void write( char v );

	// Write 8-bit integer
	void write( int8_t v );
	void write( uint8_t v );

	// Write 16-bit integer
	void write( int16_t v );
	void write( uint16_t v );

	// Write 32-bit integer
	void write( int32_t v );
	void write( uint32_t v );

	// Write 64-bit integer
	void write( int64_t v );
	void write( uint64_t v );

	// Write 32-bit floating point number
	void write( float v );

	// Write 64-bit floating point number
	void write( double v );

	// Write string
	void write( const char *v );
	void write( const std::string &v );

	// Write no-op
	void write_noop( void );

	// Begin writing a map of a fixed size
	void write_map( size_t size );

	// Begin writing an array of a fixed size
	void write_array( size_t size );

	// Begin writing a map of a unknown size
	// Must be followed by write_end()
	void write_map( void );

	// Begin writing an array of a unknown size
	// Must be followed by write_end()
	void write_array( void );

	// End a map or array of unknown size
	void write_end( void );

	// Close the writer
	hash256 close( void );

	const std::string &filename( void ) { return _filename; }

private:
	void write32( uint32_t s );
	void update( const void *data, size_t size );

	std::unique_ptr<Botan::SHA_256> _hash;
	std::string _filename;
	std::shared_ptr<std::ofstream> _file;
};

////////////////////////////////////////

