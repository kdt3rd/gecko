
#pragma once

#include <string>
#include <vector>
#include <unistd.h>
#include "stream.h"

namespace base
{

////////////////////////////////////////

class process
{
public:
	typedef pid_t id_type;

	process( void );

	void set_input( const std::string &in_file );
	void set_ouput( const std::string &out_file );
	void set_error( const std::string &err_file );
	void set_ouput_error( const std::string &out_file );

	void set_pipe( bool in, bool out, bool err );

	void execute( const std::string &exe, const std::vector<std::string> &args );

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

	id_type id( void ) const
	{
		return _id;
	}

	void kill( bool force = false );
	void wait( void );

private:
	id_type _id = 0;
	int _fdin = -1;
	int _fdout = -1;
	int _fderr = -1;
	std::unique_ptr<base::istream> _stdout;
	std::unique_ptr<base::istream> _stderr;
	std::unique_ptr<base::ostream> _stdin;
};

////////////////////////////////////////

}

