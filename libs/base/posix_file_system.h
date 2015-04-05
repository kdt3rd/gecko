
#pragma once

#include "file_system.h"

namespace base
{

////////////////////////////////////////

class posix_file_system : public file_system
{
public:
	directory_iterator readdir( const uri &path ) override;
};

////////////////////////////////////////

}

