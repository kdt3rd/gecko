
#pragma once

#include "data_reader.h"
#include "data_writer.h"

////////////////////////////////////////

class repository
{
public:
	repository( const char *repo );
	~repository( void );

	// Open a chunk for reading.
	data_reader open( const hash256 &h );

	// Create a chunk for writing.
	data_writer create( void );

	// Close a chunk that was created.
	hash256 close( data_writer &file );

private:
	std::string _repo;
};

////////////////////////////////////////

