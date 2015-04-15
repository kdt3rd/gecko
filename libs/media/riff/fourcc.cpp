
#include "fourcc.h"
#include <base/contract.h>

namespace media
{
namespace riff
{

////////////////////////////////////////

fourcc::fourcc( std::istream &in )
{
	in.read( _char, 4 );
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const fourcc &c )
{
	out << c[0] << c[1] << c[2] << c[3];
	return out;
}

////////////////////////////////////////

std::istream &operator>>( std::istream &in, fourcc &c )
{
	char ch[5];
	if ( in.read( ch, 4 ) )
		c = fourcc( ch );
	return in;
}

////////////////////////////////////////

}
}

