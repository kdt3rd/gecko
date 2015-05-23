
#pragma once

#include "uri.h"
#include "directory_iterator.h"
#include "contract.h"
#include "stream.h"
#include <functional>
#include <map>
#include <memory>

namespace base
{

////////////////////////////////////////

class file_system
{
public:
	static const std::ios_base::openmode text_read_mode = std::ios_base::in;
	static const std::ios_base::openmode file_read_mode = (std::ios_base::in|std::ios_base::binary);

	static const std::ios_base::openmode text_write_mode = (std::ios_base::out|std::ios_base::trunc);
	static const std::ios_base::openmode file_write_mode = (std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
	
	virtual ~file_system( void );

	virtual directory_iterator readdir( const uri &path ) = 0;

	virtual istream open_read( const base::uri &path,
							   std::ios_base::openmode m = file_read_mode ) = 0;
	virtual ostream open_write( const base::uri &path,
								std::ios_base::openmode m = file_write_mode ) = 0;

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

