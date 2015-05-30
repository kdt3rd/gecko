
#include "process.h"
#include "stream.h"
#include "unix_streambuf.h"
#include <sys/resource.h>
#include <unistd.h>

namespace base
{

namespace detail
{

void safe_dup( int fsrc, int fdst )
{
	while ( true )
	{
		if ( dup2( fsrc, fdst ) == -1 )
		{
			if ( errno != EBUSY || errno != EINTR )
				throw_errno( "dup2( {0}, {1} )", fsrc, fdst );
		}
		else
			break;
	}
}

class pipe
{
public:
	pipe( void )
		: _pipe{ -1, -1 }
	{
		if ( ::pipe( _pipe ) < 0 )
			throw_errno( "creating pipes" );
	}

	~pipe( void )
	{
		this->close();
	}

	int read( void ) const
	{
		return _pipe[0];
	}

	int write( void ) const
	{
		return _pipe[1];
	}

	void close_dup_read( int new_read_fd )
	{
		safe_dup( read(), new_read_fd );
		this->close();
	}

	void close_dup_write( int new_write_fd )
	{
		safe_dup( write(), new_write_fd );
		this->close();
	}

	void close( void )
	{
		::close( _pipe[0] );
		::close( _pipe[1] );
		_pipe[0] = _pipe[1] = -1;
	}

private:
	int _pipe[2];
};

}

////////////////////////////////////////

process::process( const std::string &exe, const std::vector<std::string> &args )
{
	try
	{
		detail::pipe sin, sout, serr;

		_id = fork();
		if ( _id == -1 )
			throw_errno( "fork" );

		if ( _id == 0 )
		{
			// The forked process
			try
			{
				// Close/dup the appriopriate I/O.
				sin.close_dup_read( STDIN_FILENO );
				sout.close_dup_write( STDOUT_FILENO );
				serr.close_dup_write( STDERR_FILENO );

				// Close all other files.
				struct rlimit lim;
				if ( getrlimit( RLIMIT_NOFILE, &lim ) < 0 )
					throw_errno( "getting number of file (creating process)" );
				for ( int f = 0; f < int(lim.rlim_cur); ++f )
				{
					if ( f != STDIN_FILENO && f != STDOUT_FILENO && f != STDERR_FILENO )
						::close( f );
				}

				// Create the argument list
				std::vector<char *> cargs;
				cargs.reserve( args.size() + 2 );
				cargs.push_back( const_cast<char *>( exe.c_str() ) );
				for ( auto &a: args )
					cargs.push_back( const_cast<char *>( a.c_str() ) );
				cargs.push_back( nullptr );

				// Execute!
				::execvp( cargs[0], &cargs[0] );
				throw_errno( "failed to exec {0}", exe );
			}
			catch ( std::exception &e )
			{
				base::print_exception( std::cerr, e );
			}
			exit( -1 );
		}
		else
		{
			// The normal process continues...
			auto bin = std::unique_ptr<base::ostream::streambuf_type>( new base::unix_streambuf( std::ios_base::out | std::ios_base::binary, sin.write(), true, "stdin" ) );
			_stdin = std::unique_ptr<base::ostream>( new base::ostream( std::move( bin ) ) );

			auto bout = std::unique_ptr<base::istream::streambuf_type>( new base::unix_streambuf( std::ios_base::in | std::ios_base::binary, sout.read(), true, "stdout" ) );
			_stdout = std::unique_ptr<base::istream>( new base::istream( std::move( bout ) ) );

			auto berr = std::unique_ptr<base::istream::streambuf_type>( new base::unix_streambuf( std::ios_base::in | std::ios_base::binary, serr.read(), true, "stderr" ) );
			_stderr = std::unique_ptr<base::istream>( new base::istream( std::move( berr ) ) );
		}
	}
	catch ( ... )
	{
		throw_add( "creating process {0}", exe );
	}
}

////////////////////////////////////////

void process::kill( bool force )
{
}

////////////////////////////////////////

}

