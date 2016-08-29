
#include "posix_file_system.h"
#include "contract.h"
#include "scope_guard.h"
#include "unix_streambuf.h"

#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <fstream>
#include <memory>
#include <system_error>
#include <mutex>
#include <map>
#include <tuple>
#include <thread>
#ifdef __linux
#include <sys/inotify.h>
#else
# if defined(__APPLE__)
#  include <sys/event.h>
#  include <sys/time.h>
# endif
#endif


////////////////////////////////////////


namespace
{

std::mutex theWatcherMutex;
// only need one of these per process, right?
std::shared_ptr<base::fs_watcher> theWatcher;

class posix_watcher : public base::fs_watcher
{
public:
#ifdef __APPLE__
	posix_watcher( void )
			: _notify_fd( kqueue() )
	{
		if ( _notify_fd == -1 )
			throw_errno( "Unable to initialize notification watcher" );
	}

	~posix_watcher( void )
	{
		if ( _notify_fd != -1 )
			::close( _notify_fd );
	}

	void registerWatch( const base::uri &path, base::fs_watch &w,
						base::fs_event mask, bool recursive ) override
	{
		(void)path;
		(void)w;
		(void)mask;
		(void)recursive;
		throw_runtime( "NYI: call kevent64 to register watch on a path" );
	}

	void move( base::fs_watch &w, base::fs_watch &neww ) override
	{
		(void)w;
		(void)neww;
		throw_runtime( "NYI: call move watchers" );
	}

	void unregisterWatch( base::fs_watch &w ) override
	{
		(void)w;
		throw_runtime( "NYI: remove the kevent" );
	}
#elif defined(__linux)
	posix_watcher( void )
			: _notify_fd( inotify_init1( IN_NONBLOCK|IN_CLOEXEC ) )
	{
		if ( _notify_fd == -1 )
			throw_errno( "Unable to initialize notification watcher" );

		_watch_thread = std::thread( &posix_watcher::watchThread, this );
	}
	~posix_watcher( void )
	{
		if ( _notify_fd != -1 )
			::close( _notify_fd );
	}

	void registerWatch( const base::uri &path, base::fs_watch &w,
						base::fs_event mask, bool recursive ) override
	{
		std::string p = path.full_path();
		uint32_t imask = 0;

		if ( mask & base::fs_event::CREATED )
			imask |= IN_CREATE;
		if ( mask & base::fs_event::DELETED )
			imask |= IN_DELETE;
		if ( mask & base::fs_event::MODIFIED )
			imask |= IN_MODIFY;
		if ( mask & base::fs_event::RENAMED_FROM )
			imask |= IN_MOVED_FROM;
		if ( mask & base::fs_event::RENAMED_TO )
			imask |= IN_MOVED_TO;

		int wd = inotify_add_watch( _notify_fd, p.c_str(), imask );
		if ( wd != 0 )
			throw_errno( "Unable to create file system watch on {0}", path );

		if ( recursive )
			throw_not_yet();

		std::unique_lock<std::mutex> lk( theWatcherMutex );
		_watches[wd] = std::make_tuple( path, mask, &w );
	}

	void move( base::fs_watch &w, base::fs_watch &neww ) override
	{
		std::unique_lock<std::mutex> lk( theWatcherMutex );
		bool found = false;
		for ( auto &i: _watches )
		{
			if ( std::get<2>( i.second ) == &w )
			{
				std::get<2>( i.second ) = &neww;
				found = true;
				break;
			}
		}
		precondition( found, "Unregistered watch class received in unregister" );
	}

	void unregisterWatch( base::fs_watch &w ) override
	{
		std::unique_lock<std::mutex> lk( theWatcherMutex );

		bool found = false;
		for ( auto i = _watches.begin(); i != _watches.end(); ++i )
		{
			if ( std::get<2>( i->second ) == &w )
			{
				int r = inotify_rm_watch( _notify_fd, i->first );
				if ( r != 0 )
					throw_errno( "Unable to remove file system watch on {0}", std::get<0>( i->second ) );
				_watches.erase( i );
				found = true;
				break;
			}
		}

		precondition( found, "Unregistered watch class received in unregister" );
		if ( _watches.empty() )
		{
			_watch_thread.join();
			// should be the same as delete this
			theWatcher.reset();
			return;
		}
	}

private:
	void watchThread( void )
	{
		std::unique_lock<std::mutex> lk( theWatcherMutex );
		constexpr size_t kBufSize = 16384;
//		constexpr size_t kEvtSize = sizeof(inotify_event) + NAME_MAX + 1;
//		constexpr size_t kNumEvents = kBufSize / kEvtSize;
		std::unique_ptr<uint8_t[]> buf( new uint8_t[kBufSize] );

		while ( true )
		{
			if ( _watches.empty() )
				break;

			ssize_t nRead = ::read( _notify_fd, buf.get(), kBufSize );
			if ( nRead < 0 )
			{
				if ( errno == EINTR )
					continue;
				throw_errno( "Unable to read from inotify FD" );
			}

			ssize_t cur = 0;
			uint8_t *ptr = buf.get();
			while ( cur < nRead )
			{
				struct inotify_event *inevt = reinterpret_cast<struct inotify_event *>( ptr );
				cur += inevt->len;
				if ( static_cast<size_t>( nRead - cur ) < sizeof(inotify_event) )
					throw_runtime( "Partial read of inotify stack" );

				auto w = _watches.find( inevt->wd );
				if ( w != _watches.end() )
				{
					
				}
			}
		}
		::close( _notify_fd );
		_notify_fd = -1;
	}

	std::thread _watch_thread;
	std::map< int, std::tuple<base::uri, base::fs_event, base::fs_watch *> > _watches;
#endif
	int _notify_fd;
};

std::shared_ptr<base::fs_watcher>
getWatcher( void )
{
	std::unique_lock<std::mutex> lk( theWatcherMutex );

	if ( ! theWatcher )
		theWatcher = std::make_shared<posix_watcher>();

	return theWatcher;
}

}


////////////////////////////////////////


namespace base
{

////////////////////////////////////////

uri
posix_file_system::current_path( void ) const
{
	char *tmp = ::getcwd( NULL, 0 );
	on_scope_exit{ ::free( tmp ); };
	return uri( tmp );
}

////////////////////////////////////////

void
posix_file_system::lstat( const uri &path, struct stat *buf )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::lstat( fpath.c_str(), buf ) != 0 )
		throw_errno( "retrieving file info for {0}", fpath );
}


////////////////////////////////////////


void
posix_file_system::statfs( const uri &path, struct statvfs *s )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::statvfs( fpath.c_str(), s ) != 0 )
		throw_errno( "retrieving filesystem info for {0}", fpath );
}


////////////////////////////////////////


uri
posix_file_system::readlink( const uri &path, size_t sz )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();

	if ( sz == 0 )
	{
		struct stat sbuf;
		if ( ::lstat( fpath.c_str(), &sbuf ) != 0 )
			throw std::system_error( errno, std::system_category(), fpath );

		sz = static_cast<size_t>( sbuf.st_size ) + 1;
	}
	std::unique_ptr<char[]> linkname;
	ssize_t r;
	// things might have changed size between the stat and now
	do
	{
		linkname.reset( new char[sz + 1] );
		r = ::readlink( fpath.c_str(), linkname.get(), sz + 1 );
		if ( r == -1 )
			throw std::system_error( errno, std::system_category(), fpath );

		if ( static_cast<size_t>( r ) <= sz )
			break;
		sz *= 2;
	} while ( true );

	linkname[static_cast<size_t>(r)] = '\0';
	fpath.clear();
	fpath.append( linkname.get() );
	return uri( uri::unescape( fpath ) );
}


////////////////////////////////////////


bool posix_file_system::access( const uri &path, int mode )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	return ::access( fpath.c_str(), mode ) == 0;
}


////////////////////////////////////////


directory_iterator posix_file_system::readdir( const uri &path )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	std::shared_ptr<DIR> dir( ::opendir( fpath.c_str() ), []( DIR *d ) { ::closedir( d ); } );
	if ( !dir )
		throw_errno( "opendir failed on {0}", fpath );

	// glibc deprecates readdir_r in 2.24, handle that
#if defined(__GNU_LIBRARY__) && ( __GLIBC__ > 2 || ( __GLIBC__ == 2 && __GLIBC_MINOR__ >= 24 ) ) 
	// glibc deprecates readdir_r as readdir is safe to call in a multi-threaded environment
	// apparently using an internal lock on the temporary buffers used to make system calls
	// (with different DIR)
	auto next_entry = [=]( void )
	{
		while ( true )
		{
			errno = 0;
			struct dirent *result = ::readdir( dir.get() );
			if ( result )
			{
				if ( result->d_name[0] == '.' )
				{
					if ( result->d_name[1] == '\0' ||
						 ( result->d_name[1] == '.' && result->d_name[2] == '\0' ) )
					{
						continue;
					}
				}
				return uri( path, result->d_name );
			}
			else if ( errno != 0 )
				throw_errno( "reading directory {0}", path );

			// errno == 0, no result -> end of stream
			break;
		}
		return uri();
	};
#else
	size_t n = static_cast<size_t>( std::max( fpathconf( dirfd( dir.get() ), _PC_NAME_MAX ), long(255) ) ) + 1;
	n += offsetof( struct dirent, d_name );
	std::shared_ptr<struct dirent> dir_ent( reinterpret_cast<dirent*>( new char[n] ), []( struct dirent *d ) { delete [] reinterpret_cast<char*>( d ); } );

	auto next_entry = [=]( void )
	{
		struct dirent *result = NULL;
		while ( ::readdir_r( dir.get(), dir_ent.get(), &result ) == 0 )
		{
			if ( result )
			{
				if ( result->d_name[0] == '.' )
				{
					if ( result->d_name[1] == '\0' ||
						 ( result->d_name[1] == '.' && result->d_name[2] == '\0' ) )
						continue;
				}
				
				return uri( path, result->d_name );
			}

			return uri();
		}
		throw_errno( "reading directory {0}", path );
	};
#endif
	return directory_iterator( next_entry );
}


////////////////////////////////////////


void
posix_file_system::mkdir( const uri &path, mode_t mode )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::mkdir( fpath.c_str(), mode ) == -1 )
	{
		if ( errno != EEXIST )
			throw_errno( "making directory {0}", fpath );
		else
		{
			struct stat buf = {};
			this->stat( path, &buf );
			if ( ! S_ISDIR( buf.st_mode ) )
			{
				errno = ENOTDIR;
				throw_errno( "making directory {0}", fpath );
			}
		}
	}
}


////////////////////////////////////////


void
posix_file_system::mkdir_all( const uri &path, mode_t mode )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri, got {0}", path.scheme() );
	base::uri curpath( path.root() );
	for ( auto &p: path.path() )
	{
		curpath /= p;
		if ( ::mkdir( curpath.full_path().c_str(), mode ) == -1 )
		{
			if ( errno != EEXIST )
				throw_errno( "making directory {0}", curpath );
			else
			{
				struct stat buf = {};
				this->stat( curpath, &buf );
				if ( ! S_ISDIR( buf.st_mode ) )
				{
					errno = ENOTDIR;
					throw_errno( "making directory {0}", curpath );
				}
			}
		}
	}
}


////////////////////////////////////////


void
posix_file_system::rmdir( const uri &path )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::rmdir( fpath.c_str() ) == -1 )
	{
		if ( errno != ENOENT )
			throw_errno( "removing directory {0}", fpath );
	}
}


////////////////////////////////////////


void
posix_file_system::unlink( const uri &path )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::unlink( fpath.c_str() ) == -1 )
	{
		if ( errno != ENOENT )
			throw_errno( "unlinking {0}", fpath );
	}
}


////////////////////////////////////////


void
posix_file_system::symlink( const uri &curpath, const uri &newpath )
{
	precondition( newpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string newfpath = newpath.full_path();
	std::stringstream pathbuf;
	if ( curpath.scheme() == "file" )
		pathbuf << curpath.full_path();
	else
		pathbuf << curpath;
	std::string curfpath = pathbuf.str();
	if ( ::symlink( curfpath.c_str(), newfpath.c_str() ) == -1 )
		throw_errno( "creating symlink {0} to {1}", newfpath, curfpath );
}


////////////////////////////////////////


void
posix_file_system::link( const uri &curpath, const uri &newpath )
{
	precondition( curpath.scheme() == "file", "posix_file_system expected \"file\" uri, got {0}", curpath.scheme() );
	precondition( newpath.scheme() == "file", "posix_file_system expected \"file\" uri, got {0}", newpath.scheme() );
	std::string curfpath = curpath.full_path();
	std::string newfpath = newpath.full_path();
	if ( ::link( curfpath.c_str(), newfpath.c_str() ) != 0 )
		throw_errno( "creating hard link from {0} to {1}", curfpath, newfpath );
}


////////////////////////////////////////


void
posix_file_system::rename( const uri &oldpath, const uri &newpath )
{
	precondition( oldpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	precondition( newpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string oldfpath = oldpath.full_path();
	std::string newfpath = newpath.full_path();
	if ( ::rename( oldfpath.c_str(), newfpath.c_str() ) != 0 )
		throw_errno( "renaming {0} to {1}", oldfpath, newfpath );
}


////////////////////////////////////////


istream
posix_file_system::open_read( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	istream ret( std::move( sb ) );
	ret.exceptions( std::ios_base::failbit );
	return ret;
}


////////////////////////////////////////


ostream
posix_file_system::open_write( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	ostream ret( std::move( sb ) );
	ret.exceptions( std::ios_base::failbit );
	return ret;
}


////////////////////////////////////////


iostream
posix_file_system::open( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	iostream ret( std::move( sb ) );
	ret.exceptions( std::ios_base::failbit );
	return ret;
}


////////////////////////////////////////


fs_watch
posix_file_system::watch( const base::uri &path,
						  const fs_watch::event_handler &evtcb,
						  fs_event evt_mask, bool recursive )
{
	auto fsw = getWatcher();
	fs_watch retval( fsw, evtcb );
	fsw->registerWatch( path, retval, evt_mask, recursive );
	return retval;
}


////////////////////////////////////////

}

