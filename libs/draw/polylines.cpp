
#include <iostream>
#include "polylines.h"
#include "geometry.h"
#include "mesh.h"
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

void polylines::move_to( const base::point &p )
{
	if ( _lines.empty() )
		_lines.emplace_back();
	if ( !_lines.back().empty() )
		_lines.emplace_back();

	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::line_to( const base::point &p )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::quadratic_to( const base::point &p1, const base::point &p2 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_quadratic( _lines.back().back(), p1, p2, _lines.back() );
}

////////////////////////////////////////

void polylines::cubic_to( const base::point &p1, const base::point &p2, const base::point &p3 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_cubic( _lines.back().back(), p1, p2, p3, _lines.back() );
}

////////////////////////////////////////

void polylines::arc_to( const base::point &center, double radius, double angle1, double angle2 )
{
	precondition( !_lines.empty(), "no point to start from" );
	add_arc( center, radius, angle1, angle2, _lines.back() );
}

////////////////////////////////////////

/*
void polylines::arc_to( const point &center, const point &radius, double angle1, double angle2 )
{
	_points.push_back( center );
	_points.push_back( radius );
	_points.emplace_back( angle1, angle2 );
	_actions.push_back( action::ARC );
}
*/

////////////////////////////////////////

void polylines::add_point( const base::point &p )
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

polylines polylines::stroked( double width )
{
	using namespace ClipperLib;
	Path subj;
	Paths solution;

	polylines result;
	for ( const auto &line: _lines )
	{
		if ( line.empty() )
			continue;

		subj.clear();
		for ( const auto &p: line )
			subj << IntPoint( static_cast<int>(p.x() * 100 + 0.5), static_cast<int>(p.y() * 100 + 0.5) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedLine : etOpenRound );

		solution.clear();
		co.Execute( solution, width * 50 );
		for ( auto path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.0, p.Y / 100.0 } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

polylines polylines::offset( double width )
{
	using namespace ClipperLib;
	Path subj;
	Paths solution;

	polylines result;
	for ( const auto &line: _lines )
	{
		subj.clear();
		for ( const auto &p: line )
			subj << IntPoint( static_cast<int>(p.x() * 100 + 0.5),
							  static_cast<int>(p.y() * 100 + 0.5) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedPolygon : etOpenRound );
		solution.clear();
		co.Execute( solution, width * 100 );

		for ( const auto &path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.0, p.Y / 100.0 } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

mesh<base::point> polylines::debug( void )
{
	mesh<base::point> result;
	for ( const auto &line: _lines )
	{
		if ( line.closed() )
			result.begin( gl::primitive::LINE_LOOP );
		else
			result.begin( gl::primitive::LINE_STRIP );
		for ( const auto &p: line )
			result.push_back( p );
		result.end();
	}

	return result;
}

////////////////////////////////////////

mesh<base::point> polylines::filled( void )
{
	precondition( !_lines.empty(), "no polylines" );

	using namespace ClipperLib;
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
						subj << IntPoint( static_cast<int>(p.x() * 100 + 0.5),
										  static_cast<int>(p.y() * 100 + 0.5) );
					clip.AddPath( subj, ptSubject, line.closed() );
				}
			}
		}
		clip.Execute( ctUnion, solution );
	}

	std::vector<base::point> points;
	mesh<base::point> m;
	m.begin( gl::primitive::TRIANGLES );

	auto add_point = [&]( double x, double y )
	{
		points.emplace_back( x, y );
	};

	auto add_tri = [&]( size_t a, size_t b, size_t c )
	{
		m.push_back( { points[a].x(), points[a].y() } );
		m.push_back( { points[b].x(), points[b].y() } );
		m.push_back( { points[c].x(), points[c].y() } );
	};

	tessellator tess( add_point, add_tri );
	for ( auto &poly: solution )
	{
		auto contour = tess.begin_contour();
		for ( auto p: poly )
			tess.contour_point( contour, p.X / 100.0, p.Y / 100.0 );
		tess.end_contour( contour );
	}

	tess.tessellate();

	/*
	auto *verts = tess.get_vertices();
	auto *elems = tess.get_elements();

	for ( size_t i = 0; i < tess.get_element_count(); ++i )
	{
		auto *idx = &elems[i*3];
		m.push_back( { verts[idx[0]*2], verts[idx[0]*2+1] } );
		m.push_back( { verts[idx[1]*2], verts[idx[1]*2+1] } );
		m.push_back( { verts[idx[2]*2], verts[idx[2]*2+1] } );
	}
	*/

	m.end();

	return m;
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

