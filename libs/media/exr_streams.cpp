
#include "exr_streams.h"
#include <iostream>

namespace imgio
{

////////////////////////////////////////

exr_istream::exr_istream( std::istream &str )
	: IStream( "" ), _stream( str )
{
}

////////////////////////////////////////

bool exr_istream::read( char c[], int n )
{
	return bool( _stream.read( c, n ) );
}

////////////////////////////////////////

Imf::Int64 exr_istream::tellg( void )
{
	return _stream.tellg();
}

////////////////////////////////////////

void exr_istream::seekg( Imf::Int64 pos )
{
	_stream.seekg( pos );
}

////////////////////////////////////////

void exr_istream::clear( void )
{
	_stream.clear();
}

////////////////////////////////////////

}
