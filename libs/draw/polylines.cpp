
#include <iostream>
#include <cstdint>
#include "polylines.h"
#include "geometry.h"
#include "clipper.h"
#include "tessellator.h"
#include <base/contract.h>
#include <base/timer.h>
#include <base/scope_guard.h>

namespace draw
{

////////////////////////////////////////

polylines::polylines( void )
{
}

////////////////////////////////////////

void polylines::new_polyline( void )
{
	if ( _lines.empty() )
		_lines.emplace_back();
	if ( !_lines.back().empty() )
		_lines.emplace_back();
}

////////////////////////////////////////

void polylines::move_to( const gl::vec2 &p )
{
	if ( _lines.empty() )
		_lines.emplace_back();
	if ( !_lines.back().empty() )
		_lines.emplace_back();

	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::line_to( const gl::vec2 &p )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::quadratic_to( const gl::vec2 &p1, const gl::vec2 &p2 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_quadratic( _lines.back().back(), p1, p2, _lines.back() );
}

////////////////////////////////////////

void polylines::cubic_to( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_cubic( _lines.back().back(), p1, p2, p3, _lines.back() );
}

////////////////////////////////////////

void polylines::arc_to( const gl::vec2 &center, float radius, float angle1, float angle2 )
{
	precondition( !_lines.empty(), "no point to start from" );
	add_arc( center, radius, angle1, angle2, _lines.back() );
}

////////////////////////////////////////

/*
void polylines::arc_to( const point &center, const point &radius, float angle1, float angle2 )
{
	_points.push_back( center );
	_points.push_back( radius );
	_points.emplace_back( angle1, angle2 );
	_actions.push_back( action::ARC );
}
*/

////////////////////////////////////////

void polylines::add_point( const gl::vec2 &p )
{
	precondition( !_lines.empty(), "no point to start from" );
	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::close( void )
{
	precondition( !_lines.empty(), "no polyline to close" );
	_lines.back().close();
	_lines.emplace_back();
}

////////////////////////////////////////

polylines polylines::stroked( float width )
{
	using namespace draw::detail;
	Path subj;
	Paths solution;

	polylines result;
	for ( const auto &line: _lines )
	{
		if ( line.empty() )
			continue;

		subj.clear();
		for ( const auto &p: line )
			subj << IntPoint( static_cast<int>( p[0] * 100 + 0.5F ), static_cast<int>( p[1] * 100 + 0.5F ) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedLine : etOpenRound );

		solution.clear();
		co.Execute( solution, width * 50 );
		for ( auto path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.F, p.Y / 100.F } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

polylines polylines::offset( float width )
{
	using namespace draw::detail;
	Path subj;
	Paths solution;

	polylines result;
	for ( const auto &line: _lines )
	{
		subj.clear();
		for ( const auto &p: line )
			subj << IntPoint( static_cast<int>( p[0] * 100 + 0.5F ),
							  static_cast<int>( p[1] * 100 + 0.5F ) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedPolygon : etOpenRound );
		solution.clear();
		co.Execute( solution, width * 100 );

		for ( const auto &path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.F, p.Y / 100.F } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

void polylines::filled( const std::function<void(float,float)> &add_point, const std::function<void(size_t,size_t,size_t)> &add_tri )
{
	precondition( !_lines.empty(), "no polylines" );

	using namespace draw::detail;
	Paths solution;
	{
		Clipper clip;
		clip.StrictlySimple( true );
		{
			Path subj;
			for ( const auto &line: _lines )
			{
				if ( !line.empty() )
				{
					subj.clear();
					for ( const auto &p: line )
						subj << IntPoint( static_cast<int>( p[0] * 100 + 0.5F ), static_cast<int>( p[1] * 100 + 0.5F ) );
					clip.AddPath( subj, ptSubject, line.closed() );
				}
			}
		}
		clip.Execute( ctUnion, solution );
	}

	tessellator tess( add_point, add_tri );
	for ( auto &poly: solution )
	{
		auto contour = tess.begin_contour();
		for ( auto p: poly )
			tess.contour_point( contour, p.X / 100.0, p.Y / 100.0 );
		tess.end_contour( contour );
	}

	tess.tessellate();
}

////////////////////////////////////////

void polylines::save_svg( std::ostream &out )
{
	out << "<svg>\n";
	for ( auto &p: _lines )
	{
		out << "  ";
		p.save_svg_polyline( out );
		p.save_svg_point_numbers( out );
		out << '\n';
	}
	out << "</svg>\n";
}

////////////////////////////////////////

}

