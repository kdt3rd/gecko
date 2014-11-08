
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <fstream>
#include <imgproc/token.h>
#include <imgproc/parser.h>
#include <imgproc/cpp_generator.h>
#include <imgproc/decl.h>
#include <imgproc/environment.h>
#include <map>
#include <memory>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option( 'h', "help", "",       base::cmd_line::arg<0>, "Print help message and exit", false ),
		base::cmd_line::option( 'p', "path", "<path>", base::cmd_line::multi,  "Include path", true ),
		base::cmd_line::option(  0,  "",     "<func>, ...", base::cmd_line::args,   "Test file", true )
	);

	auto option_error = base::make_guard( [&]()
	{
		std::cerr << options << std::endl;
	} );

	options.parse( argc, argv );

	option_error.dismiss();

	if ( options["help"] )
	{
		std::cerr << options << std::endl;
		return 0;
	}

	std::vector<std::shared_ptr<imgproc::function>> funcs;
	for ( auto &path: options["path"].values() )
	{
		std::ifstream src( path );
		imgproc::parser parser( funcs, src );

		parser.parse();
		for ( auto msg: parser.messages() )
			std::cout << msg << std::endl;

		if ( parser.has_errors() )
			throw_runtime( "ERROR: parsing {0}", path );
	}

	imgproc::environment env( funcs );
	for ( auto &func: options["<func>, ..."].values() )
	{
		std::stringstream str( func );
		imgproc::decl d;
		imgproc::iterator tok( str );
		d.parse( tok );

		std::vector<imgproc::type_operator> args;
		for ( auto &a: d.get_type() )
			args.push_back( a.get<imgproc::type_operator>() );

		for ( auto &f: funcs )
		{
			if ( d.name() == f->name() )
			{
				std::cout << "Compiling: " << d << std::endl;
				auto r = env.infer( *f, args );
				std::cout << r->get_type() << std::endl;
				break;
			}
		}
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
