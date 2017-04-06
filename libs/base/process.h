//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <vector>
#include <atomic>
#include <functional>
#include <unistd.h>
#include "stream.h"

namespace base
{

////////////////////////////////////////

class process
{
public:
	typedef pid_t id_t;

	process( void );
	~process( void );

	void set_input( const std::string &in_file );
	void set_output( const std::string &out_file );
	void set_error( const std::string &err_file );
	void set_output_error( const std::string &out_file );

	void set_pipe( bool in, bool out, bool err );

	void execute( const std::string &exe, const std::vector<std::string> &args );

	void set_callback( const std::function<void(void)> &cb );

	base::ostream &std_in( void )
	{
		precondition( _stdin, "null input stream" );
		return *_stdin.get();
	}

	std::istream &std_out( void )
	{
		precondition( _stdout, "null output stream" );
		return *_stdout.get();
	}

	std::istream &std_err( void )
	{
		precondition( _stderr, "null error stream" );
		return *_stderr.get();
	}

	void close_in( void )
	{
		_stdin.reset();
	}

	void close_out( void )
	{
		_stdout.reset();
	}

	void close_err( void )
	{
		_stderr.reset();
	}

	id_t id( void ) const
	{
		return _id;
	}

	bool exited( void )
	{
		return _exited;
	}

	bool signaled( void )
	{
		return _signaled;
	}

	uint8_t exit_status( void )
	{
		return _exit_status;
	}

	int exit_signal( void )
	{
		return _exit_signal;
	}

	void terminate( bool force = false );

	void wait( void );

private:
	static void collect_zombies( void );
	void update_status( int status );

	id_t _id = 0;
	int _fdin = -1;
	int _fdout = -1;
	int _fderr = -1;
	std::unique_ptr<base::istream> _stdout;
	std::unique_ptr<base::istream> _stderr;
	std::unique_ptr<base::ostream> _stdin;

	std::atomic<bool> _exited{ false };
	std::atomic<bool> _signaled{ false };

	std::atomic<uint8_t> _exit_status{ 0 };
	std::atomic<int> _exit_signal{ 0 };

	std::function<void(void)> _callback;
};

////////////////////////////////////////

}

