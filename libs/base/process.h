
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

	process( const std::string &exe, const std::vector<std::string> &args );

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

	id_type id( void ) const
	{
		return _id;
	}

	void kill( bool force = false );
	void wait( void );

private:
	id_type _id;
	std::unique_ptr<base::istream> _stdout;
	std::unique_ptr<base::istream> _stderr;
	std::unique_ptr<base::ostream> _stdin;
};

////////////////////////////////////////

}

