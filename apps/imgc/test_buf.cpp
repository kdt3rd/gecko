
#include <base/contract.h>
#include <iomanip>
#include <fstream>
#include <imgproc/buffer.h>
#include <map>
#include <memory>
#include "png_reader.h"

imgproc::buffer<int64_t,1> test( imgproc::buffer<float,2> &input );
imgproc::buffer<float,1> uniform_buckets( int64_t nbuckets, float lo, float hi );

namespace
{

int safemain( int argc, char *argv[] )
{
	auto img = png_read( "/home/iangodin/Pictures/mandril.png" );

	/*
	std::cout << "image: " << img.size( 0 ) << 'x' << img.size( 1 ) << std::endl;
	std::cout << "    X: " << img.lower( 0 ) << '-' << img.upper( 0 ) << std::endl;
	std::cout << "    Y: " << img.lower( 1 ) << '-' << img.upper( 1 ) << std::endl;
	for ( size_t y = img.lower( 1 ); y < img.upper( 1 ); y += 4 )
	{
		for ( size_t x = img.lower( 0 ); x < img.upper( 0 ); x += 4 )
		{
			float p = img( x, y );
			if ( p < 0.25 )
				std::cout << '-';
			else if ( p < 0.5 )
				std::cout << 'O';
			else if ( p < 0.75 )
				std::cout << '@';
			else
				std::cout << '#';
		}
		std::cout << std::endl;
	}
	*/

	/*
	auto  tmp = uniform_buckets( 256, 0.F, 1.F );
	for ( int64_t i = tmp.lower( 0 ); i < tmp.upper( 0 ); ++i )
		std::cout << tmp( i )  << ' ' << int64_t( tmp( i ) * 255.F ) << std::endl;
	*/

	auto histo = test( img );

	for ( int64_t i = histo.lower( 0 ); i < histo.upper( 0 ); ++i )
		std::cout << std::setw( 3 ) << i << ' ' << histo( i ) << ' ' << std::string( histo( i ) / 10, '*' ) << '\n';
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
