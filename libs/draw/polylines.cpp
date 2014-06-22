
#include <iostream>
#include "polylines.h"
#include "geometry.h"
#include "mesh.h"
#include "clipper.h"
#include "libtess2/tesselator.h"
#include <core/contract.h>
#include <core/scope_guard.h>

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

void polylines::move_to( const core::point &p )
{
	if ( _lines.empty() )
		_lines.emplace_back();
	if ( !_lines.back().empty() )
		_lines.emplace_back();

	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::line_to( const core::point &p )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	_lines.back().push_back( p );
}

////////////////////////////////////////

void polylines::quadratic_to( const core::point &p1, const core::point &p2 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_quadratic( _lines.back().back(), p1, p2, _lines.back() );
}

////////////////////////////////////////

void polylines::cubic_to( const core::point &p1, const core::point &p2, const core::point &p3 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_cubic( _lines.back().back(), p1, p2, p3, _lines.back() );
}

////////////////////////////////////////

void polylines::arc_to( const core::point &center, double radius, double angle1, double angle2 )
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

void polylines::add_point( const core::point &p )
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
		subj.clear();
		for ( const auto &p: line )
			subj << IntPoint( p.x() * 100 + 0.5, p.y() * 100 + 0.5 );

		ClipperOffset co;
		solution.clear();
		if ( line.closed() )
		{
			result._lines.push_back( line );
			co.AddPath( subj, jtRound, etClosedPolygon );
			co.Execute( solution, -width * 100 );
		}
		else
		{
			co.AddPath( subj, jtRound, etOpenRound );
			co.Execute( solution, width * 50 );
		}

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
			subj << IntPoint( p.x() * 100 + 0.5, p.y() * 100 + 0.5 );

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

mesh<core::point> polylines::debug( void )
{
	mesh<core::point> result;
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

namespace {
	void *stdAlloc( void *data, unsigned int size )
	{
		return malloc( size );
	}

	void stdFree( void *data, void *ptr )
	{
		free( ptr );
	}

}

mesh<core::point> polylines::filled( void )
{
	precondition( !_lines.empty(), "no polylines" );

	using namespace ClipperLib;
	Path subj;
	Clipper clip;
	for ( const auto &line: _lines )
	{
		if ( !line.empty() )
		{
			subj.clear();
			for ( const auto &p: line )
				subj << IntPoint( p.x() * 100 + 0.5, p.y() * 100 + 0.5 );
			clip.AddPath( subj, ptSubject, line.closed() );
		}
	}

	Paths solution;
	clip.Execute( ctUnion, solution );

	TESSalloc ma;
	memset( &ma, 0, sizeof(ma) );
	ma.memalloc = &stdAlloc;
	ma.memfree = &stdFree;

	TESStesselator *tess = tessNewTess( &ma );
	if ( !tess )
		throw std::runtime_error( "tesselator creation failed" );
	on_scope_exit += [=]() { tessDeleteTess( tess ); };

	std::vector<TESSreal> tmp;
	for ( auto path: solution )
	{
		tmp.clear();
		for ( size_t p = 0; p < path.size(); ++p )
		{
			tmp.push_back( path[p].X / 100.0 );
			tmp.push_back( path[p].Y / 100.0 );
		}
		tessAddContour( tess, 2, tmp.data(), sizeof(float)*2, tmp.size()/2 );
	}

	if ( !tessTesselate( tess, TESS_WINDING_POSITIVE, TESS_POLYGONS, 3, 2, nullptr ) )
		throw std::runtime_error( "tesselation failed" );

	const float *verts = tessGetVertices( tess );
	const int *elems = tessGetElements( tess );
	const int nelems = tessGetElementCount( tess );

	// TODO use element array when it's added to the GL library
	mesh<core::point> m;
	m.begin( gl::primitive::TRIANGLES );
	for ( int i = 0; i < nelems; ++i )
	{
		const int *p = &elems[i*3];
		for ( int j = 0; j < 3 && p[j] != TESS_UNDEF; ++j )
			m.push_back( { verts[p[j]*2], verts[p[j]*2+1] } );
	}
	m.end();

	return m;
}

////////////////////////////////////////

}

