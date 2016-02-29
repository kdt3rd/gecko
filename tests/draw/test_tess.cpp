
#include <base/contract.h>
#include <base/unit_test.h>
#include <draw/clipper.h>
#include <draw/tessellator.h>

#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0] );

	base::unit_test test( "tess" );
	test.setup( options );

	options.add_help();
	options.parse( argc, argv );

	if ( options["help"] )
	{
		std::cerr << options << std::endl;
		return -1;
	}

	test["holes"] = [&]( void )
	{
		using namespace ClipperLib;
		Clipper clip;

		Path subj;
		subj.push_back( IntPoint( 0, 0 ) );
		subj.push_back( IntPoint( 0, 100 ) );
		subj.push_back( IntPoint( 100, 100 ) );
		subj.push_back( IntPoint( 100, 0 ) );
		clip.AddPath( subj, ptSubject, true );

		subj.clear();
		subj.push_back( IntPoint( 20, 20 ) );
		subj.push_back( IntPoint( 80, 20 ) );
		subj.push_back( IntPoint( 80, 80 ) );
		subj.push_back( IntPoint( 20, 80 ) );
		clip.AddPath( subj, ptSubject, true );

		PolyTree solution;
		clip.Execute( ctUnion, solution );

		draw::tessellator tess;
		for ( auto *poly: solution.Childs )
		{
			draw::tessellator::node *n = tess.triangulate( poly );
			auto print_polygon = []( const base::point &p )
			{
				std::cout << p << std::endl;
			};

			tess.process_polygon( n, print_polygon );
		}

		test.success( "holes successful" );
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
