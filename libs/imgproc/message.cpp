//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "message.h"
#include <utf/utf.h>

namespace imgproc
{

////////////////////////////////////////

message::message( msg_type t, const location &l, const std::string &m )
	: _type( t ), _location( l ), _msg( m )
{
}

////////////////////////////////////////

void message::write( std::ostream &out ) const
{
	switch ( _type )
	{
		case MSG_INFO:
			out << "notice ";
			break;

		case MSG_WARNING:
			out << "warning ";
			break;

		case MSG_ERROR:
			out << "error ";
			break;
	}
	out << "at " << _location << ": " << _msg;
}

////////////////////////////////////////

}

