
#pragma once

#include <ImfIO.h>

namespace imgio
{

////////////////////////////////////////

class exr_istream : public Imf::IStream
{
public:
	exr_istream( std::istream &str );

	bool read( char c[], int n ) override;

	Imf::Int64 tellg( void ) override;

	void seekg( Imf::Int64 pos ) override;

	void clear( void ) override;

//	bool isMemoryMapped( void ) const override;

//	char *readMemoryMapped( int n ) override;

private:
	std::istream &_stream;
};

////////////////////////////////////////

}
