// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT

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
	if ( _lines.empty() || !_lines.back().empty() )
		_lines.emplace_back();
}

////////////////////////////////////////

void polylines::move_to( const polyline::point &p )
{
	new_polyline();
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

void polylines::arc_to( const polyline::point &center, dim radius, float angle1, float angle2 )
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
}

////////////////////////////////////////

polylines polylines::stroked( dim width, dim dx, dim dy, dim dz ) const
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
			subj << IntPoint( static_cast<int>( to_api( p[0] ) * 100.F + 0.5F ),
			                  static_cast<int>( to_api( p[1] ) * 100.F + 0.5F ),
							  static_cast<int>( to_api( p[2] ) * 100.F + 0.5F ) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedLine : etOpenRound );

		solution.clear();
		co.Execute( solution, to_api( width * 50.F ) );
		auto adx = to_api( dx );
		auto ady = to_api( dy );
		auto adz = to_api( dz );
		for ( auto path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.F + adx, p.Y / 100.F + ady, p.Z / 100.F + adz } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

polylines polylines::offset( dim width, dim dx, dim dy, dim dz ) const
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
			subj << IntPoint( static_cast<int>( to_api( p[0] ) * 100.F + 0.5F ),
			                  static_cast<int>( to_api( p[1] ) * 100.F + 0.5F ),
							  static_cast<int>( to_api( p[2] ) * 100.F + 0.5F ) );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedPolygon : etOpenRound );

		solution.clear();
		co.Execute( solution, to_api( width * 100.F ) );
		auto adx = to_api( dx );
		auto ady = to_api( dy );
		auto adz = to_api( dz );
		for ( const auto &path: solution )
		{
			result.new_polyline();
			for ( const auto &p: path )
				result.add_point( { p.X / 100.F + adx, p.Y / 100.F + ady, p.Z / 100.F + adz } );
			result.close();
		}
	}

	return result;
}

////////////////////////////////////////

void polylines::filled( const std::function<void(float,float)> &add_point, const std::function<void(uint32_t,uint32_t,uint32_t)> &add_tri ) const
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
						subj << IntPoint( static_cast<int>( to_api( p[0] ) * 100.F + 0.5F ),
						                  static_cast<int>( to_api( p[1] ) * 100.F + 0.5F ) );
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
			tess.contour_point( contour, p.X / 100.F, p.Y / 100.F, p.Z / 100.F );
		tess.end_contour( contour );
	}

	tess.tessellate();
}

////////////////////////////////////////

void polylines::filled( gl::mesh &m, const std::string &attr ) const
{
	gl::vertex_buffer_data<gl::vec3> points;
	gl::element_buffer_data tris;

	auto add_point = [&]( float cx, float cy )
	{
		points.push_back( { cx, cy, 0.F } ); // TODO: handle Z
	};

	auto add_tri = [&]( uint32_t a, uint32_t b, uint32_t c )
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
