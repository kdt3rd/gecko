
#include <base/contract.h>
#include <iomanip>
#include <fstream>
#include <imgproc/buffer.h>
#include <map>
#include <memory>
#include "png_reader.h"

using namespace imgproc;

buffer<int64_t,1> test( const buffer<float,2> &input );

namespace
{

int safemain( int argc, char *argv[] )
{
	auto img = png_read( "/home/iangodin/Pictures/mandril.png" );

	auto histo = test( img );

	for ( int64_t i = histo.lower( 0 ); i < histo.upper( 0 ); ++i )
		std::cout << base::format( "{0,w3} {1,w4} {2}\n", i, histo( i ), std::string( histo( i ) / 25, '*' ) );
	std::cout << std::flush;

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
