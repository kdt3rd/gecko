//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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

void polylines::move_to( const polyline::point &p )
{
	if ( _lines.empty() )
		_lines.emplace_back();
	if ( !_lines.back().empty() )
		_lines.emplace_back();

	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::line_to( const polyline::point &p )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::quadratic_to( const polyline::point &p1, const polyline::point &p2 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_quadratic( _lines.back().back(), p1, p2, _lines.back() );
}

////////////////////////////////////////

void polylines::cubic_to( const polyline::point &p1, const polyline::point &p2, const polyline::point &p3 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_cubic( _lines.back().back(), p1, p2, p3, _lines.back() );
}

////////////////////////////////////////

void polylines::arc_to( const polyline::point &center, float radius, float angle1, float angle2 )
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

void polylines::add_point( const polyline::point &p )
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

polylines polylines::stroked( float width, float dx, float dy, float dz ) const
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
			subj << IntPoint( static_cast<int>( p[0] * 100 + 0.5F ), static_cast<int>( p[1] * 100 + 0.5F ), static_cast<int>( p[2] * 100 + 0.5F ) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedLine : etOpenRound );

		solution.clear();
		co.Execute( solution, width * 50 );
		for ( auto path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.F + dx, p.Y / 100.F + dy, p.Z / 100.F + dz } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

polylines polylines::offset( float width, float dx, float dy, float dz ) const
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
			subj << IntPoint( static_cast<int>( p[0] * 100 + 0.5F ), static_cast<int>( p[1] * 100 + 0.5F ), static_cast<int>( p[2] * 100 + 0.5F ) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedPolygon : etOpenRound );

		solution.clear();
		co.Execute( solution, width * 100 );
		for ( const auto &path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.F + dx, p.Y / 100.F + dy, p.Z / 100.F + dz } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

void polylines::filled( const std::function<void(float,float)> &add_point, const std::function<void(size_t,size_t,size_t)> &add_tri ) const
{
	precondition( !_lines.empty(), "no polylines" );

	// Simplify and clean up the polygons
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

	// Tessellate into triangles
	tessellator tess( add_point, add_tri );
	for ( auto &poly: solution )
	{
		auto contour = tess.begin_contour();
		for ( auto p: poly )
			tess.contour_point( contour, p.X / 100.0, p.Y / 100.0, p.Z / 100.0 );
		tess.end_contour( contour );
	}

	tess.tessellate();
}

////////////////////////////////////////

void polylines::filled( gl::mesh &m, const std::string &attr ) const
{
	gl::vertex_buffer_data<polyline::point> points;
	gl::element_buffer_data tris;

	auto add_point = [&]( float cx, float cy )
	{
		points.push_back( { cx, cy, 0 } ); // TODO handle Z
	};

	auto add_tri = [&]( size_t a, size_t b, size_t c )
	{
		tris.push_back( a, b, c );
	};

	filled( add_point, add_tri );

	auto bind = m.bind();
	bind.vertex_attribute( attr, points );
	bind.set_elements( tris );
	m.add_triangles( tris.size() );
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

