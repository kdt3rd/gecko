
#include <cmath>
#include <vector>
#include <core/contract.h>
#include "painter.h"

namespace platform
{

////////////////////////////////////////

painter::painter( void )
{
}

////////////////////////////////////////

painter::~painter( void )
{
}

////////////////////////////////////////

std::shared_ptr<points> painter::make_star( double x, double y, double radius, int npoints, int density )
{
	precondition( npoints > 2, "star must have 3 or more points" );
	precondition( density > 1, "star density must be at least 2" );

	const double PI = 2 * std::acos( 0.0 );
	auto ret = new_points();

	// Create the list of points
	std::vector<std::pair<double,double>> points( npoints );
	double part = PI * 2.0 / double(npoints);
	for ( int p = 0; p < npoints; ++p )
		points[p] = std::make_pair( x + radius * sin( part * p ), y - radius * cos( part * p ) );

	// Loop over all points un
	bool done = false;
	int p = 0;
	while ( !done )
	{
		ret->add_point( points[p] );
		p = ( p + density ) % npoints;
		if ( p == 0 )
			done = true;
	}
	ret->add_point( points.front() );

	return ret;
}

////////////////////////////////////////

}

