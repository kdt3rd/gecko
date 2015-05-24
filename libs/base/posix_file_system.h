
#pragma once

#include "file_system.h"

namespace base
{

////////////////////////////////////////

class posix_file_system : public file_system
{
public:
	const size_t page_size = 4096;

	directory_iterator readdir( const uri &path ) override;

	istream open_read( const base::uri &path, std::ios_base::openmode m = file_system::file_read_mode ) override;
	ostream open_write( const base::uri &path, std::ios_base::openmode m = file_system::file_write_mode ) override;
};

////////////////////////////////////////

}

