//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "process.h"
#include "stream.h"
#include "unix_streambuf.h"
#include "scope_guard.h"
#include <mutex>
#include <condition_variable>
#include <map>
#include <thread>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

namespace base
{

namespace
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
		close();
	}
	pipe( const pipe & ) = delete;
	pipe( const pipe && ) = delete;
	pipe &operator=( const pipe & ) = delete;
	pipe &operator=( pipe && ) = delete;

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

std::mutex the_mutex;
std::condition_variable the_condition;
std::map<process::id_t,process*> the_processes;

}

////////////////////////////////////////

process::process( void )
{
}

////////////////////////////////////////

process::~process( void )
{
	if ( _id != 0 )
	{
		auto tmp = _id;
		try
		{
			terminate( true );
		}
		catch ( ... )
		{
			// Silently try to kill the process
		}

		std::unique_lock<std::mutex> lock( the_mutex );
		the_processes.erase( tmp );
	}
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

void process::set_output( const std::string &out_file )
{
	_stdout.reset();
	if ( _fdout >= 0 )
		::close( _fdout );

	_fdout = ::open( out_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644 );
	if ( _fdout < 0 )
		throw_errno( "opening process output ({0})", out_file );
}

////////////////////////////////////////

void process::set_error( const std::string &err_file )
{
	_stderr.reset();
	if ( _fderr >= 0 )
		::close( _fderr );

	_fderr = ::open( err_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644 );
	if ( _fderr < 0 )
		throw_errno( "opening process error ({0})", err_file );
}

////////////////////////////////////////

void process::set_output_error( const std::string &out_file )
{
	_stdout.reset();
	_stderr.reset();
	if ( _fdout >= 0 )
		::close( _fdout );
	if ( _fderr >= 0 )
		::close( _fderr );

	_fdout = ::open( out_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644 );
	if ( _fdout < 0 )
		throw_errno( "opening process output/error ({0})", out_file );
	_fderr = safe_dup( _fdout );
}

////////////////////////////////////////

void process::set_pipe( bool in, bool out, bool err )
{
	if ( in )
	{
		if ( _fdin >= 0 )
			::close( _fdin );
		pipe sin;
		_fdin = sin.steal_read();
		auto bin = std::unique_ptr<base::ostream::streambuf_type>( new base::unix_streambuf( std::ios_base::out | std::ios_base::binary, sin.steal_write(), false, "stdin" ) );
		_stdin = std::unique_ptr<base::ostream>( new base::ostream( std::move( bin ) ) );
	}

	if ( out )
	{
		if ( _fdout >= 0 )
			::close( _fdout );
		pipe sout;
		_fdout = sout.steal_write();
		auto bout = std::unique_ptr<base::istream::streambuf_type>( new base::unix_streambuf( std::ios_base::in | std::ios_base::binary, sout.steal_read(), false, "stdout" ) );
		_stdout = std::unique_ptr<base::istream>( new base::istream( std::move( bout ) ) );
	}

	if ( err )
	{
		if ( _fderr >= 0 )
			::close( _fderr );
		pipe serr;
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
					safe_dup( _fdin, STDIN_FILENO );
				if ( _fdout >= 0 )
					safe_dup( _fdout, STDOUT_FILENO );
				if ( _fderr >= 0 )
					safe_dup( _fderr, STDERR_FILENO );

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
			std::unique_lock<std::mutex> lock( the_mutex );
			if ( the_processes.empty() )
			{
				std::thread collector( []( void ) { collect_zombies(); } );
				collector.detach();
			}
			the_processes[_id] = this;
		}
	}
	catch ( ... )
	{
		throw_add( "creating process {0}", exe );
	}
}

////////////////////////////////////////

void process::set_callback( const std::function<void(void)> &cb )
{
	std::unique_lock<std::mutex> lock( the_mutex );
	_callback = cb;
	if ( _exited || _signaled )
	{
		try
		{
			_callback();
		}
		catch ( ... )
		{
		}
	}
}

////////////////////////////////////////

void process::terminate( bool force )
{
	if ( _id > 0 )
	{
		if ( force )
			::kill( _id, SIGKILL );
		else
			::kill( _id, SIGTERM );
	}
}

////////////////////////////////////////

void process::wait( void )
{
	std::unique_lock<std::mutex> lock( the_mutex );
	the_condition.wait( lock, [=]( void ){ return !this->signaled() && !this->exited(); } );
}

////////////////////////////////////////

void process::update_status( int status )
{
	_exited = WIFEXITED( status );
	if ( _exited )
		_exit_status = WEXITSTATUS( status );
	else
		_exit_status = 0;

	_signaled = WIFSIGNALED( status );
	if ( _signaled )
		_exit_signal = WTERMSIG( status );
	else
		_exit_signal = 0;

	_id = 0;

	if ( _callback )
	{
		std::cout << "Callback" << std::endl;
		try
		{
			_callback();
		}
		catch ( ... )
		{
		}
	}
}

////////////////////////////////////////

void process::collect_zombies( void )
{
	while ( true )
	{
		int status = 0;
		int ret = ::wait( &status );
		if ( ret == -1 )
		{
			if ( errno == EINTR )
				continue;
			if ( errno == ECHILD )
			{
				std::unique_lock<std::mutex> lock( the_mutex );
				if ( the_processes.empty() )
					return;
				else
					break;
			}
			throw_errno( "waiting on child processes" );
		}
		else
		{
			std::unique_lock<std::mutex> lock( the_mutex );
			auto i = the_processes.find( pid_t(ret) );
			if ( i != the_processes.end() )
			{
				i->second->update_status( status );
				the_processes.erase( i );
				the_condition.notify_all();
			}
		}
	}
}

////////////////////////////////////////

}

