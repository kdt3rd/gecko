//
// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <base/contract.h>
#include <iomanip>
#include <fstream>
#include <imgproc/buffer.h>
#include <map>
#include <memory>
#include "png_reader.h"

using namespace imgproc;

//buffer<float,2> test( const buffer<float,2> &input );
buffer<float,1> test( void );

namespace
{

std::ostream &operator<<( std::ostream &out, const buffer<float,2> &img )
{
	int64_t inc = img.size(0) / 64; 
	for ( int64_t y = img.lower(1); y < img.upper(1); y += inc*2 )
	{
		for ( int64_t x = img.lower(0); x < img.upper(0); x += inc )
		{
			if ( img( x, y ) < 0.1 )
				out << " ";
			else if ( img( x, y ) < 0.25 )
				out << "░";
			else if ( img( x, y ) < 0.5 )
				out << "▒";
			else if ( img( x, y ) < 0.75 )
				out << "▓";
			else
				out << "█";
		}
		out << '\n';
	}
	return out;
}

std::ostream &operator<<( std::ostream &out, const buffer<float,1> &img )
{
	int64_t inc = img.size(0) / 64; 
	for ( int64_t x = img.lower(0); x < img.upper(0); x += inc )
		out << " " << x << ' ' << img(x) << '\n';
	return out;
}

int safemain( int argc, char *argv[] )
{
//	auto img = png_read( "/home/iangodin/Pictures/mandril.png" );

	auto result = test();

//	std::cout << img << std::endl;;
	std::cout << result << std::endl;;

	float sum = 0.F;
	for ( int64_t i = result.lower( 0 ); i < result.upper( 0 ); ++i )
		sum += result( i );

	std::cerr << "Sum = " << sum << std::endl;

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
