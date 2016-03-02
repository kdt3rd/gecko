
#include <base/contract.h>
#include <base/unit_test.h>
#include <base/math_functions.h>
#include <base/path.h>
#include <draw/clipper.h>
#include <draw/polylines.h>
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

		Paths solution;
		clip.Execute( ctUnion, solution );

		tessellator tess;
		for ( auto &poly: solution )
		{
			auto contour = tess.begin_contour();
			for ( auto &p: poly )
				tess.contour_point( contour, p.X * 100.0, p.Y * 100.0 );
			tess.end_contour( contour );
		}
		tess.tessellate();
		test.success( "successful" );
	};

	test["star"] = [&]( void )
	{
		using namespace base::math;
		base::point center { 500, 500 };
		double side = 450;
		std::vector<base::point> points;
		size_t p = 5;
		size_t q = 2;
		for ( size_t i = 0; i < p; ++i )
			points.push_back( center + base::point::polar( side, 360_deg * double(i) / double(p) ) );

		base::path path;
		size_t i = q % points.size();
		path.move_to( points[0] );
		while( i != 0 )
		{
			path.line_to( points[i] );
			i = ( i + q ) % points.size();
		}
		path.close();

		draw::polylines lines;
		path.replay( lines );

//		lines.save_svg( std::cout );
		draw::polylines tmp = lines.stroked( 10 );
		tmp.save_svg( std::cout );

		auto mesh = lines.stroked( 10 ).filled();
		test.success( "successful" );
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
