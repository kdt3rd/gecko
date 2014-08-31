
#include <base/contract.h>
#include <fstream>
#include <imgproc/token.h>
#include <imgproc/parser.h>
#include <imgproc/cpp_generator.h>
#include <map>
#include <memory>

namespace
{

int safemain( int argc, char *argv[] )
{
	precondition( argc == 3, "expected two argument: input and output" );

	std::ifstream src( argv[1] );

	std::vector<std::shared_ptr<imgproc::func>> funcs;
	imgproc::parser parser( funcs, src );

	parser();
	for ( auto msg: parser.messages() )
		std::cout << msg << std::endl;

	if ( parser.has_errors() )
		throw_runtime( "ERROR: parsing {0}", argv[1] );

	for ( auto f: funcs )
	{
		f->write( std::cout );
		std::cout << std::endl;
	}

	std::ofstream cppout( argv[2] );
	imgproc::cpp_generator gen( cppout );
	gen.add_functions( funcs );

	std::vector<imgproc::type> args
	{
		{ imgproc::data_type::FLOAT32, 2 }
	};

	gen.generate( U"test", args );

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
