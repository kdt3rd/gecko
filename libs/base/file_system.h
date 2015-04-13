
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
	virtual ~file_system( void );

	virtual directory_iterator readdir( const uri &path ) = 0;

	virtual std::unique_ptr<std::istream> open_read( const base::uri &path ) = 0;
	virtual std::unique_ptr<std::ostream> open_write( const base::uri &path ) = 0;
	virtual std::unique_ptr<std::ostream> create( const base::uri &path ) = 0;

	static std::shared_ptr<file_system> get( const uri &path )
	{
		precondition( path, "invalid uri" );
		precondition( _fs.find( path.scheme() ) != _fs.end(), "no file system for scheme" );
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

