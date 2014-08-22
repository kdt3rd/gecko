
#include <base/contract.h>
#include <fstream>
#include <imgproc/token.h>
#include <imgproc/parser.h>
#include <map>
#include <memory>

namespace
{

int safemain( int argc, char *argv[] )
{
	precondition( argc == 2, "expected single argument" );

	std::ifstream src( argv[1] );

	/*
	imgproc::iterator token( src );
	while ( token.next() )
		std::cout << token << '\n';
	*/

	std::vector<std::shared_ptr<imgproc::func>> funcs;
	imgproc::parser parser( funcs, src );

	try
	{
		parser();
	}
	catch ( ... )
	{
		for ( auto msg: parser.messages() )
			std::cout << msg << std::endl;
		throw;
	}

	for ( auto msg: parser.messages() )
		std::cout << msg << std::endl;

	std::cout << "Got " << funcs.size() << " functions" << std::endl;
	for ( auto f: funcs )
	{
		f->write( std::cout );
		std::cout << std::endl;
	}

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
