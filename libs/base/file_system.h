
#pragma once

#include "uri.h"
#include "directory_iterator.h"
#include "contract.h"
#include <functional>
#include <map>
#include <memory>

namespace base
{

////////////////////////////////////////

class file_system
{
public:
	virtual ~file_system( void )
	{
	}

	virtual directory_iterator readdir( const uri &path ) = 0;

	/*
	std::istream open_for_read( const base::uri &u );
	std::ostream open_for_write( const base::uri &u );
	std::ostream create( const base::uri &u );
	*/

	static std::shared_ptr<file_system> get( const uri &path )
	{
		precondition( path, "invalid uri" );
		return _fs.at( path.scheme() );
	}

	static void add( const std::string &sch, const std::shared_ptr<file_system> &fs )
	{
		_fs[sch] = fs;
	}

private:
	static std::map<std::string,std::shared_ptr<file_system>> _fs;

};

////////////////////////////////////////

}

