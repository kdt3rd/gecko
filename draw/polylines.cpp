
#include <iostream>
#include "polylines.h"
#include "geometry.h"
#include "mesh.h"
#include "clipper.h"
#include "libtess2/tesselator.h"
#include <core/contract.h>

namespace 
{
	constexpr double PI = 3.14159265358979323846;
}

namespace draw
{

////////////////////////////////////////

polylines::polylines( void )
{
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
	throw std::runtime_error( "not yet implemented" );
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
	for ( size_t i = 0; i < _lines.size(); ++i )
	{
		const polyline &line = _lines[i];
		subj.clear();
		for ( size_t p = 0; p < line.size(); ++p )
			subj << IntPoint( line[p].x() * 100 + 0.5, line[p].y() * 100 + 0.5 );
		if ( line.closed() )
			subj << IntPoint( line.front().x() * 100 + 0.5, line.front().y() * 100 + 0.5 );
		ClipperOffset co;
		co.AddPath( subj, jtRound, etOpenRound ); //line.closed() ? etClosedPolygon : etOpenRound );
		solution.clear();
		co.Execute( solution, width * 100 );

		for ( auto path: solution )
		{
			CleanPolygon( path );
			result.move_to( { path[0].X / 100.0, path[0].Y / 100.0 } );
			for ( size_t p = 1; p < path.size(); ++p )
				result.line_to( { path[p].X / 100.0, path[p].Y / 100.0 } );
			result.close();
		}
	}

	std::cout << "Stroked: " << result << std::endl;

	return result; //.filled();
}

////////////////////////////////////////

polylines polylines::offset( double width )
{
	using namespace ClipperLib;
	Path subj;
	Paths solution;

	polylines result;
	for ( size_t i = 0; i < _lines.size(); ++i )
	{
		const polyline &line = _lines[i];
		subj.clear();
		for ( size_t p = 0; p < line.size(); ++p )
			subj << IntPoint( line[p].x() * 100 + 0.5, line[p].y() * 100 + 0.5 );

		ClipperOffset co;
		co.AddPath( subj, jtRound, line.closed() ? etClosedPolygon : etOpenRound );
		solution.clear();
		co.Execute( solution, width * 100 );

		for ( auto path: solution )
		{
			CleanPolygon( path );
			result.move_to( { path[0].X / 100.0, path[0].Y / 100.0 } );
			for ( size_t p = 1; p < path.size(); ++p )
				result.line_to( { path[p].X / 100.0, path[p].Y / 100.0 } );
			result.close();
		}
	}

	std::cout << "Offset: " << result << std::endl;

	return result; //.filled();
}

////////////////////////////////////////

mesh<core::point> polylines::stroked( void )
{
	mesh<core::point> result;
	for ( size_t i = 0; i < _lines.size(); ++i )
	{
		const polyline &line = _lines[i];
		if ( line.closed() )
			result.begin( gl::primitive::LINE_LOOP );
		else
			result.begin( gl::primitive::LINE_STRIP );
		for ( size_t p = 0; p < line.size(); ++p )
			result.push_back( line[p] );
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
	if ( _lines.empty() )
		throw std::runtime_error( "no path" );
	while ( _lines.back().empty() )
		_lines.pop_back();
	if ( _lines.empty() )
		throw std::runtime_error( "no path" );

	using namespace ClipperLib;
	Path subj;

	Clipper clip;
	for ( size_t i = 0; i < _lines.size(); ++i )
	{
		const polyline &line = _lines[i];
		subj.clear();
		for ( size_t p = 0; p < line.size(); ++p )
			subj << IntPoint( line[p].x() * 100 + 0.5, line[p].y() * 100 + 0.5 );
		clip.AddPath( subj, ptSubject, line.closed() );
	}

	Paths solution;
	clip.Execute( ctUnion, solution );
//	path result;

	/*
	mesh<core::point> ret;
	for ( auto path: solution )
	{
		ret.begin( gl::primitive::LINE_LOOP );
		for ( size_t p = 0; p < path.size(); ++p )
			ret.push_back( { path[p].X / 100.0, path[p].Y / 100.0 } );
		ret.end();
	}
	return ret;
	*/

	TESSalloc ma;
	memset( &ma, 0, sizeof(ma) );
	ma.memalloc = &stdAlloc;
	ma.memfree = &stdFree;

	TESStesselator *tess = tessNewTess( &ma );
	if ( !tess )
		throw std::runtime_error( "tesselator creation failed" );
//	on_scope_exit += [=]() { tessDeleteTess( tess ); }

	std::vector<TESSreal> tmp;
	int nvp = 3;
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

	if ( !tessTesselate( tess, TESS_WINDING_POSITIVE, TESS_POLYGONS, nvp, 2, nullptr ) )
		throw std::runtime_error( "tesselation failed" );

	const float *verts = tessGetVertices( tess );
//	const int *vinds = tessGetVertexIndices( tess );
	const int *elems = tessGetElements( tess );
	const int nverts = tessGetVertexCount( tess );
	const int nelems = tessGetElementCount( tess );

	std::cout << "NVerts = " << nverts << ' ' << TESS_UNDEF << std::endl;
	std::cout << "NElems = " << nelems << std::endl;
	mesh<core::point> m;
	for ( int i = 0; i < nelems; ++i )
	{
		const int *p = &elems[i*nvp];
		m.begin( gl::primitive::TRIANGLE_STRIP );
		for ( int j = 0; j < nvp && p[j] != TESS_UNDEF; ++j )
			m.push_back( { verts[p[j]*2], verts[p[j]*2+1] } );
//		m.push_back( { verts[p[0]*2], verts[p[0]*2+1] } );
		m.end();
	}

	return m;
}

////////////////////////////////////////

}

