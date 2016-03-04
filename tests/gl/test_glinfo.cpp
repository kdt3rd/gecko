
#include <base/contract.h>
#include <base/unit_test.h>

#include <platform/platform.h>
#include <platform/system.h>
#include <gl/context.h>

#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0] );

	base::unit_test test( "info" );
	test.setup( options );

	options.add_help();
	options.parse( argc, argv );

	if ( options["help"] )
	{
		std::cerr << options << std::endl;
		return -1;
	}

	auto sys = platform::platform::common().create();
	auto win = sys->new_window();

	win->acquire();

	test["version"] = [&]( void )
	{
		gl::context ctxt;
		test.success( "{0}", ctxt.get_version() );
	};

	test["vendor"] = [&]( void )
	{
		gl::context ctxt;
		test.success( "{0}", ctxt.get_vendor() );
	};

	test["renderer"] = [&]( void )
	{
		gl::context ctxt;
		test.success( "{0}", ctxt.get_renderer() );
	};

	test["shading_version"] = [&]( void )
	{
		gl::context ctxt;
		test.success( "{0}", ctxt.get_shading_version() );
	};

	test["uniform_buffer"] = [&]( void )
	{
		gl::context ctxt;
		test.success( "{0}", ctxt.get_max_uniform_buffer_bindings() );
	};

	test.run( options );
	test.clean();

	return - static_cast<int>( test.failure_count() );
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
		std::cerr << '\n';
		base::print_exception( std::cerr, e );
	}
}
