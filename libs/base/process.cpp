
#include "process.h"
#include "stream.h"
#include "unix_streambuf.h"
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

namespace base
{

namespace detail
{

int safe_dup( int fsrc )
{
	int result = -1;
	while ( result < 0 )
	{
		result = dup( fsrc );
		if ( result == -1 )
		{
			if ( errno != EBUSY || errno != EINTR )
				throw_errno( "dup( {0} )", fsrc );
		}
	}

	return result;
}

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

	void close( void )
	{
		::close( _pipe[0] );
		::close( _pipe[1] );
		_pipe[0] = _pipe[1] = -1;
	}

	int steal_read( void )
	{
		int result = -1;
		std::swap( result, _pipe[0] );
		return result;
	}

	int steal_write( void )
	{
		int result = -1;
		std::swap( result, _pipe[1] );
		return result;
	}

private:
	int _pipe[2];
};

}

////////////////////////////////////////

process::process( void )
{
}

////////////////////////////////////////

void process::set_input( const std::string &in_file )
{
	_stdin.reset();
	if ( _fdin >= 0 )
		::close( _fdin );

	_fdin = ::open( in_file.c_str(), O_RDONLY );
	if ( _fdin < 0 )
		throw_errno( "opening process input ({0})", in_file );
}

////////////////////////////////////////

void process::set_ouput( const std::string &out_file )
{
	_stdout.reset();
	if ( _fdout >= 0 )
		::close( _fdout );

	_fdout = ::open( out_file.c_str(), O_WRONLY | O_APPEND );
	if ( _fdout < 0 )
		throw_errno( "opening process output ({0})", out_file );
}

////////////////////////////////////////

void process::set_error( const std::string &err_file )
{
	_stderr.reset();
	if ( _fderr >= 0 )
		::close( _fderr );

	_fderr = ::open( err_file.c_str(), O_RDONLY );
	if ( _fderr < 0 )
		throw_errno( "opening process error ({0})", err_file );
}

////////////////////////////////////////

void process::set_ouput_error( const std::string &out_file )
{
	_stdout.reset();
	_stderr.reset();
	if ( _fdout >= 0 )
		::close( _fdout );
	if ( _fderr >= 0 )
		::close( _fderr );

	_fdout = ::open( out_file.c_str(), O_WRONLY | O_APPEND );
	if ( _fdout < 0 )
		throw_errno( "opening process output/error ({0})", out_file );
	_fderr = detail::safe_dup( _fdout );
}

////////////////////////////////////////

void process::set_pipe( bool in, bool out, bool err )
{
	if ( in )
	{
		if ( _fdin >= 0 )
			::close( _fdin );
		detail::pipe sin;
		_fdin = sin.steal_read();
		auto bin = std::unique_ptr<base::ostream::streambuf_type>( new base::unix_streambuf( std::ios_base::out | std::ios_base::binary, sin.steal_write(), false, "stdin" ) );
		_stdin = std::unique_ptr<base::ostream>( new base::ostream( std::move( bin ) ) );
	}

	if ( out )
	{
		if ( _fdout >= 0 )
			::close( _fdout );
		detail::pipe sout;
		_fdout = sout.steal_write();
		auto bout = std::unique_ptr<base::istream::streambuf_type>( new base::unix_streambuf( std::ios_base::in | std::ios_base::binary, sout.steal_read(), false, "stdout" ) );
		_stdout = std::unique_ptr<base::istream>( new base::istream( std::move( bout ) ) );
	}

	if ( err )
	{
		if ( _fderr >= 0 )
			::close( _fderr );
		detail::pipe serr;
		_fderr = serr.steal_write();
		auto berr = std::unique_ptr<base::istream::streambuf_type>( new base::unix_streambuf( std::ios_base::in | std::ios_base::binary, serr.steal_read(), false, "stderr" ) );
		_stderr = std::unique_ptr<base::istream>( new base::istream( std::move( berr ) ) );
	}
}

////////////////////////////////////////

void process::execute( const std::string &exe, const std::vector<std::string> &args )
{
	try
	{
		_id = fork();
		if ( _id == -1 )
			throw_errno( "process fork" );

		if ( _id == 0 )
		{
			// The forked process
			try
			{
				// Close/dup the appriopriate I/O.
				if ( _fdin >= 0 )
					detail::safe_dup( _fdin, STDIN_FILENO );
				if ( _fdout >= 0 )
					detail::safe_dup( _fdout, STDOUT_FILENO );
				if ( _fderr >= 0 )
					detail::safe_dup( _fderr, STDERR_FILENO );

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

