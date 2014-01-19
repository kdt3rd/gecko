
#include "geometry.h"
#include <tuple>

namespace
{
	template<int n, typename T>
	inline const core::point &pt( const T &t )
	{
		return std::get<n>( t );
	}

	constexpr double PI = 3.14159265358979323846;
}


namespace draw
{

////////////////////////////////////////

size_t circle_precision( double r )
{
	size_t n = 3;
	double error = 0.0;
	do
	{
		n = n * 2;
		error = r * ( 1 - std::cos( PI / n ) );
	} while ( error > 1.0 );

	return n;
}


////////////////////////////////////////

void add_quadratic( const core::point &p1, const core::point &p2, const core::point &p3, polyline &line )
{
	using core::point;
	typedef std::tuple<point,point,point> curve;
	std::vector<curve> stack;
	stack.reserve( 16 );
	stack.emplace_back( p1, p2, p3 );
	point c0, c1, c2, c3;

	while ( !stack.empty() )
	{
		curve &c = stack.back();
		point mid = ( pt<0>( c ) + pt<2>( c ) ) * 0.5;

		if ( point::distance_squared( mid, pt<1>( c ) ) <= 0.01 )
		{
			line.push_back( pt<2>( c ) );
			stack.pop_back();
		}
		else
		{
			c0 = pt<0>( c );
			c1 = ( c0 + pt<1>( c ) ) * 0.5;
			c3 = ( pt<1>( c ) + pt<2>( c ) ) * 0.5;
			c2 = ( c1 + c3 ) * 0.5;

			c = std::make_tuple( c2, c3, pt<2>( c ) ); // right side
			stack.emplace_back( c0, c1, c2 );
		}
	}
}

////////////////////////////////////////

void add_cubic( const core::point &p1, const core::point &p2, const core::point &p3, const core::point &p4, polyline &line )
{
	using core::point;
	typedef std::tuple<point,point,point,point> curve;
	std::vector<curve> stack;
	stack.reserve( 16 );
	stack.emplace_back( p1, p2, p3, p4 );
	point p0, p01, p12, p23, p012, p123, p0123, d, d13, d23;
	bool first = true;

	while ( !stack.empty() )
	{
		curve &c = stack.back();
		p01 = ( pt<0>( c ) + pt<1>( c ) ) * 0.5;
		p12 = ( pt<1>( c ) + pt<2>( c ) ) * 0.5;
		p23 = ( pt<2>( c ) + pt<3>( c ) ) * 0.5;
		p012 = ( p01 + p12 ) * 0.5;
		p123 = ( p12 + p23 ) * 0.5;
		p0123 = ( p012 + p123 ) * 0.5;

		d = pt<0>( c ) - pt<3>( c );
		d13 = pt<1>( c ) - pt<3>( c );
		d23 = pt<2>( c ) - pt<3>( c );

		double d2 = d13.x() * d.y() - d13.y() * d.x();
		double d3 = d23.x() * d.y() - d23.y() * d.x();

		if ( ( d2 + d3 ) * ( d2 + d3 ) < 0.1 * ( d.x() * d.x() + d.y() * d.y() ) && !first )
		{
			line.push_back( pt<3>( c ) );
			stack.pop_back();
		}
		else
		{
			p0 = pt<0>( c );
			c = std::make_tuple( p0123, p123, p23, pt<3>( c ) ); // right side
			stack.emplace_back( p0, p01, p012, p0123 );
		}
		first = false;
	}
}

////////////////////////////////////////

void add_arc( const core::point &center, double radius, double a1, double a2, polyline &line )
{
	using core::point;

	size_t n = circle_precision( radius );

	double span = std::abs( std::fmod( ( a1 - a2 + PI ), 2.0 * PI ) - PI );
	n = size_t( std::ceil( n * span / ( 2.0 * PI ) ) );

	point p = point::polar( radius, a1 ) + center;
	if ( line.empty() || point::distance_squared( p, line.back() ) > 0.1 )
		line.push_back( p );

	for ( size_t i = 1; i <= n; ++i )
	{
		double m = double(i) / double(n);
		double a = a1 * ( 1.0 - m ) + a2 * m;
		line.push_back( point::polar( radius, a ) + center );
	}
}

////////////////////////////////////////

}

