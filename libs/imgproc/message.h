
#pragma once

#include <string>
#include "location.h"

namespace imgproc
{

////////////////////////////////////////

enum msg_type
{
	MSG_INFO,
	MSG_WARNING,
	MSG_ERROR
};

////////////////////////////////////////

class message
{
public:
	message( msg_type t, const location &l, const std::string &m );

	msg_type type( void ) { return _type; }

	const location &where( void ) { return _location; }
	const std::string &get_message( void ) const { return _msg; }
	
	void write( std::ostream &out ) const;

private:
	msg_type _type;
	location _location;
	std::string _msg;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const message &m )
{
	m.write( out );
	return out;
}
////////////////////////////////////////

}

