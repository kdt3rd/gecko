
#pragma once

#include "file_system.h"

namespace base
{

////////////////////////////////////////

class posix_file_system : public file_system
{
public:
	directory_iterator readdir( const uri &path ) override;

	std::unique_ptr<std::istream> open_read( const base::uri &path ) override;
	std::unique_ptr<std::ostream> open_write( const base::uri &path ) override;
	std::unique_ptr<std::ostream> create( const base::uri &path ) override;
};

////////////////////////////////////////

}

