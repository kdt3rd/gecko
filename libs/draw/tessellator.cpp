//
// SGI FREE SOFTWARE LICENSE B (Version 2.0, Sept. 18, 2008)
// Copyright (C) [dates of first publication] Silicon Graphics, Inc.
// All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice including the dates of first publication and either this
// permission notice or a reference to http://oss.sgi.com/projects/FreeB/ shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL SILICON GRAPHICS, INC.
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Silicon Graphics, Inc. shall not
// be used in advertising or otherwise to promote the sale, use or other dealings in
// this Software without prior written authorization from Silicon Graphics, Inc.
//

// Author: Eric Veach, July 1994.

#include "tessellator.h"

#include <base/contract.h>

#include <stddef.h>
#include <stdexcept>
#include <cmath>
#include <functional>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

namespace draw
{

// START OF DICT.H
// Search returns the node with the smallest key greater than or equal
// to the given key.  If there is no such key, returns a node whose
// key is nullptr.  Similarly, Succ(Max(d)) has a nullptr key, etc.
template<typename Key>
class dict
{
public:
	class node
	{
	public:
		node *next( void )
		{
			return _next;
		}

		node *prev( void )
		{
			return _prev;
		}

		Key key( void ) const
		{
			return _key;
		}

	private:
		friend dict<Key>;

		Key	_key = Key();
		node *_next = nullptr;
		node *_prev = nullptr;
	};

	dict( const std::function<bool(Key,Key)> &comp )
		: _comp( comp )
	{
		_head._key = nullptr;
		_head._next = _head._prev = &_head;
	}

	node *search( Key key )
	{
		node *n = &_head;
		do
		{
			n = n->next();
		} while ( n->key() != nullptr && !_comp( key, n->key() ) );

		return n;
	}

	node *insert_before( node *n, Key key )
	{
		do
		{
			n = n->prev();
		} while ( n->key() != nullptr && !_comp( n->key(), key ) );

		node *newNode = _node_pool.allocate();
		newNode->_key = key;
		newNode->_next = n->next();
		newNode->_prev = n;
		n->_next->_prev = newNode;
		n->_next = newNode;

		return newNode;
	}

	void erase( node *n )
	{
		n->_next->_prev = n->_prev;
		n->_prev->_next = n->_next;
		_node_pool.deallocate( n );
	}

	node *min( void )
	{
		return _head.next();
	}

	node *max( void )
	{
		return _head.prev();
	}

	node *insert( Key k )
	{
		return insert_before( &_head, k );
	}

private:
	node _head;
	base::memory_pool<node,512> _node_pool;
	std::function<bool(Key,Key)> _comp;
};
// END OF DICT.H

// START OF MESH.H

const size_t TESS_UNDEF = ~size_t(0);
class face;

// The mesh structure is similar in spirit, notation, and operations
// to the "quad-edge" structure (see L. Guibas and J. Stolfi, Primitives
// for the manipulation of general subdivisions and the computation of
// Voronoi diagrams, ACM Transactions on Graphics, 4(2):74-123, April 1985).
// For a simplified description, see the course notes for CS348a,
// "Mathematical Foundations of Computer Graphics", available at the
// Stanford bookstore (and taught during the fall quarter).
// The implementation also borrows a tiny subset of the graph-based approach
// use in Mantyla's Geometric Work Bench (see M. Mantyla, An Introduction
// to Sold Modeling, Computer Science Press, Rockville, Maryland, 1988).
//
// The fundamental data structure is the "half-edge".  Two half-edges
// go together to make an edge, but they point in opposite directions.
// Each half-edge has a pointer to its mate (the "symmetric" half-edge Sym),
// its origin vertex (Org), the face on its left side (Lface), and the
// adjacent half-edges in the CCW direction around the origin vertex
// (Onext) and around the left face (Lnext).  There is also a "next"
// pointer for the global edge list (see below).
//
// The notation used for mesh navigation:
//  Sym   = the mate of a half-edge (same edge, but opposite direction)
//  Onext = edge CCW around origin vertex (keep same origin)
//  Dnext = edge CCW around destination vertex (keep same dest)
//  Lnext = edge CCW around left face (dest becomes new origin)
//  Rnext = edge CCW around right face (origin becomes new dest)
//
// "prev" means to substitute CW for CCW in the definitions above.
//
// The mesh keeps global lists of all vertices, faces, and edges,
// stored as doubly-linked circular lists with a dummy header node.
// The mesh stores pointers to these dummy headers (vHead, fHead, eHead).
//
// The circular edge list is special; since half-edges always occur
// in pairs (e and e->Sym), each half-edge stores a pointer in only
// one direction.  Starting at eHead and following the e->next pointers
// will visit each *edge* once (ie. e or e->Sym, but not both).
// e->Sym stores a pointer in the opposite direction, thus it is
// always true that e->Sym->next->Sym->next == e.
//
// Each vertex has a pointer to next and previous vertices in the
// circular list, and a pointer to a half-edge with this vertex as
// the origin (nullptr if this is the dummy header).
//
// Each face has a pointer to the next and previous faces in the
// circular list, and a pointer to a half-edge with this face as
// the left face (nullptr if this is the dummy header).
//
// Note that what we call a "face" is really a loop; faces may consist
// of more than one loop (ie. not simply connected), but there is no
// record of this in the data structure.  The mesh may consist of
// several disconnected regions, so it may not be possible to visit
// the entire mesh by starting at a half-edge and traversing the edge
// structure.
//
// The mesh does NOT support isolated vertices; a vertex is deleted along
// with its last edge.  Similarly when two faces are merged, one of the
// faces is deleted (see tessMeshDelete below).  For mesh operations,
// all face (loop) and vertex pointers must not be nullptr.  However, once
// mesh manipulation is finished, meshZapFace can be used to delete
// faces of the mesh, one at a time.  All external faces can be "zapped"
// before the mesh is returned to the client; then a nullptr face indicates
// a region which is not part of the output polygon.

template<typename T>
class geom_base
{
public:
	T *next( void ) const
	{
		return _next;
	}

	T *prev( void ) const
	{
		return _prev;
	}

	half_edge *edge( void ) const
	{
		return _edge;
	}

	void set_next( T *n )
	{
		_next = n;
	}

	void set_prev( T *p )
	{
		_prev = p;
	}

	void set_edge( half_edge *e )
	{
		_edge = e;
	}

	size_t index( void ) const
	{
		return _index;
	}

	void set_index( size_t i )
	{
		_index = i;
	}

private:
	T *_next; // next item (never nullptr)
	T *_prev; // previous item (never nullptr)
	half_edge *_edge; // a half-edge with this origin
	size_t _index; // to allow identify unique items
};

class vertex : public geom_base<vertex>
{
public:
	double x( void ) const
	{
		return _x;
	}

	double y( void ) const
	{
		return _y;
	}

	void set_x( double x )
	{
		_x = x;
	}

	void set_y( double y )
	{
		_y = y;
	}

	void set( double x, double y )
	{
		_x = x;
		_y = y;
	}

private:
	double _x, _y; // projection onto the sweep plane
};

class face : public geom_base<face>
{
public:
	face *trail( void ) const
	{
		return _trail;
	}

	void set_trail( face *t )
	{
		_trail = t;
	}

	bool marked( void ) const
	{
		return _marked;
	}

	bool inside( void ) const
	{
		return _inside;
	}

	void set_marked( bool m = true )
	{
		_marked = m;
	}

	void set_inside( bool i = true )
	{
		_inside = i;
	}

private:
	face *_trail; // "stack" for conversion to strips
	bool _marked; // flag for conversion to strips
	bool _inside; // this face is in the polygon interior
};

class half_edge
{
public:
	half_edge *next( void ) const { return _next; }
	half_edge *prev( void ) const { return _sym->_next; }
	half_edge *sym( void ) const { return _sym; }

	void next( half_edge *n ) { _next = n; }
	void prev( half_edge *p ) { _sym->_next = p; }
	void sym( half_edge *s ) { _sym = s; }

	half_edge *onext( void ) const { return _onext; }
	half_edge *oprev( void ) const { return sym()->lnext(); }

	half_edge *rnext( void ) const { return oprev()->sym(); }
	half_edge *rprev( void ) const { return sym()->onext(); }

	void onext( half_edge *o ) { _onext = o; }
	void oprev( half_edge *p ) { sym()->onext( p ); }

	half_edge *lnext( void ) const { return _lnext; }
	half_edge *lprev( void ) const { return onext()->sym(); }

	void lnext( half_edge *l ) { _lnext = l; }
	void lprev( half_edge *p ) { sym()->_lnext = p; }

	half_edge *dprev( void ) const { return lnext()->sym(); }
	half_edge *dnext( void ) const { return rprev()->sym(); }

	vertex *org( void ) const { return _org; }
	vertex *dst( void ) const { return sym()->org(); }

	void org( vertex *o ) { _org = o; }
	void dst( vertex *d ) { sym()->org( d ); }

	face *lface( void ) const { return _lface; }
	face *rface( void ) const { return sym()->lface(); }

	void lface( face *f ) { _lface = f; }
	void rface( face *f ) { sym()->lface( f ); }

	int winding( void ) const { return _winding; }
	void winding( int w ) { _winding = w; }
	void add_winding( int w ) { _winding += w; }

	active_region *region( void ) const { return _active_region; }
	void region( active_region *r ) { _active_region = r; }

private:
	half_edge *_next = nullptr;      // doubly-linked list (prev==Sym->next)
	half_edge *_sym = nullptr;       // same edge, opposite direction
	half_edge *_onext = nullptr;     // next edge CCW around origin
	half_edge *_lnext = nullptr;     // next edge CCW around left face
	vertex *_org = nullptr;       // origin vertex (Overtex too long)
	face *_lface = nullptr;     // left face

	active_region *_active_region = nullptr;  // a region with this upper edge (sweep.c)
	int _winding = 0;    // change in winding number when crossing from the right face to the left face
};

// START OF GEOM.H
namespace
{

inline bool VertEq( const vertex *u, const vertex *v )
{
	return std::equal_to<double>()( u->x(), v->x() ) && std::equal_to<double>()( u->y(), v->y() );
}

inline bool VertLeq( const vertex *u, const vertex *v )
{
	return ( u->x() < v->x() ) || ( std::equal_to<double>()( u->x(), v->x() ) && ( u->y() <= v->y() ) );
}

inline bool TransLeq( const vertex *u, const vertex *v )
{
	return ( u->y() < v->y() ) || ( std::equal_to<double>()( u->y(), v->y() ) && u->x() <= v->x() );
}

inline bool EdgeGoesLeft( const half_edge *e )
{
	return VertLeq( e->dst(), e->org() );
}

inline bool EdgeGoesRight( half_edge *e )
{
	return VertLeq( e->org(), e->dst() );
}

inline double VertL1dist( vertex *u, vertex *v )
{
	return std::abs( u->x() - v->x() ) + std::abs( u->y() - v->y() );
}

// Given three vertices u,v,w such that VertLeq(u,v) && VertLeq(v,w),
// evaluates the t-coord of the edge uw at the s-coord of the vertex v.
// Returns v->y() - (uw)(v->x()), ie. the signed distance from uw to v.
// If uw is vertical (and thus passes thru v), the result is zero.
//
// The calculation is extremely accurate and stable, even when v
// is very close to u or w.  In particular if we set v->y() = 0 and
// let r be the negated result (this evaluates (uw)(v->x())), then
// r is guaranteed to satisfy MIN(u->y(),w->y()) <= r <= MAX(u->y(),w->y()).
double EdgeEval( vertex *u, vertex *v, vertex *w )
{
	precondition( VertLeq( u, v ) && VertLeq( v, w ), "points not ordered" );

	double gapL = v->x() - u->x();
	double gapR = w->x() - v->x();

	if ( gapL + gapR > 0 )
	{
		if ( gapL < gapR )
			return ( v->y() - u->y() ) + ( u->y() - w->y() ) * ( gapL / ( gapL + gapR ) );
		else
			return ( v->y() - w->y() ) + ( w->y() - u->y() ) * ( gapR / ( gapL + gapR ) );
	}
	// vertical line
	return 0;
}

// Returns a number whose sign matches EdgeEval(u,v,w) but which
// is cheaper to evaluate.  Returns > 0, == 0 , or < 0
// as v is above, on, or below the edge uw.
double EdgeSign( vertex *u, vertex *v, vertex *w )
{
	precondition( VertLeq( u, v ) && VertLeq( v, w ), "points not ordered" );

	double gapL = v->x() - u->x();
	double gapR = w->x() - v->x();

	if ( gapL + gapR > 0 )
		return ( v->y() - w->y() ) * gapL + ( v->y() - u->y() ) * gapR;

	// vertical line
	return 0;
}

// Given three vertices u,v,w such that TransLeq(u,v) && TransLeq(v,w),
// evaluates the t-coord of the edge uw at the s-coord of the vertex v.
// Returns v->x() - (uw)(v->y()), ie. the signed distance from uw to v.
// If uw is vertical (and thus passes thru v), the result is zero.
//
// The calculation is extremely accurate and stable, even when v
// is very close to u or w.  In particular if we set v->x() = 0 and
// let r be the negated result (this evaluates (uw)(v->y())), then
// r is guaranteed to satisfy MIN(u->x(),w->x()) <= r <= MAX(u->x(),w->x()).
double TransEval( vertex *u, vertex *v, vertex *w )
{
	assert( TransLeq( u, v ) && TransLeq( v, w ) );

	double gapL = v->y() - u->y();
	double gapR = w->y() - v->y();

	if ( gapL + gapR > 0 )
	{
		if ( gapL < gapR )
			return ( v->x() - u->x() ) + ( u->x() - w->x() ) * gapL / ( gapL + gapR );
		else
			return ( v->x() - w->x() ) + ( w->x() - u->x() ) * gapR / ( gapL + gapR );
	}
	// vertical line
	return 0;
}

// Returns a number whose sign matches TransEval(u,v,w) but which
// is cheaper to evaluate.  Returns > 0, == 0 , or < 0
// as v is above, on, or below the edge uw.
double TransSign( vertex *u, vertex *v, vertex *w )
{
	double gapL, gapR;

	assert( TransLeq( u, v ) && TransLeq( v, w ) );

	gapL = v->y() - u->y();
	gapR = w->y() - v->y();

	if ( gapL + gapR > 0 )
		return ( v->x() - w->x() ) * gapL + ( v->x() - u->x() ) * gapR;
	// vertical line
	return 0;
}

// For almost-degenerate situations, the results are not reliable.
// Unless the floating-point arithmetic can be performed without
// rounding errors, *any* implementation will give incorrect results
// on some degenerate inputs, so the client must have some way to
// handle this situation.
bool VertCCW( vertex *u, vertex *v, vertex *w )
{
	return ( u->x() * ( v->y() - w->y() ) + v->x() * ( w->y() - u->y() ) + w->x() * ( u->y() - v->y() ) ) >= 0;
}

// Given parameters a,x,b,y returns the value (b*x+a*y)/(a+b),
// or (x+y)/2 if a==b==0.  It requires that a,b >= 0, and enforces
// this in the rare case that one argument is slightly negative.
// The implementation is extremely stable numerically.
// In particular it guarantees that the result r satisfies
// MIN(x,y) <= r <= MAX(x,y), and the results are very accurate
// even when a and b differ greatly in magnitude.
inline double Interpolate( double a, double x, double b, double y )
{
	return (a = (a < 0) ? 0 : a, b = (b < 0) ? 0 : b,
	 ((a <= b) ? (std::equal_to<double>()(b, 0) ? ((x+y) / 2)
				  : (x + (y-x) * (a/(a+b))))
	  : (y + (x-y) * (b/(a+b)))));
}

// Given edges (o1,d1) and (o2,d2), compute their point of intersection.
// The computed point is guaranteed to lie in the intersection of the
// bounding rectangles defined by each edge.
void tesedgeIntersect( vertex *o1, vertex *d1, vertex *o2, vertex *d2, vertex *v )
{
	double z1, z2;

	// This is certainly not the most efficient way to find the intersection
	// of two line segments, but it is very numerically stable.
	//
	// Strategy: find the two middle vertices in the VertLeq ordering,
	// and interpolate the intersection s-value from these.  Then repeat
	// using the TransLeq ordering to find the intersection t-value.

	if ( ! VertLeq( o1, d1 ) )
		std::swap( o1, d1 );
	if ( ! VertLeq( o2, d2 ) )
		std::swap( o2, d2 );
	if ( ! VertLeq( o1, o2 ) )
	{
		std::swap( o1, o2 );
		std::swap( d1, d2 );
	}

	if ( ! VertLeq( o2, d1 ) )
	{
		// Technically, no intersection -- do our best
		v->set_x( ( o2->x() + d1->x() ) / 2.0 );
	}
	else if ( VertLeq( d1, d2 ) )
	{
		// Interpolate between o2 and d1
		z1 = EdgeEval( o1, o2, d1 );
		z2 = EdgeEval( o2, d1, d2 );
		if ( z1 + z2 < 0 )
		{
			z1 = -z1;
			z2 = -z2;
		}
		v->set_x( Interpolate( z1, o2->x(), z2, d1->x() ) );
	}
	else
	{
		// Interpolate between o2 and d2
		z1 = EdgeSign( o1, o2, d1 );
		z2 = -EdgeSign( o1, d2, d1 );
		if ( z1 + z2 < 0 )
		{
			z1 = -z1;
			z2 = -z2;
		}
		v->set_x( Interpolate( z1, o2->x(), z2, d2->x() ) );
	}

	// Now repeat the process for t

	if ( ! TransLeq( o1, d1 ) )
		std::swap( o1, d1 );
	if ( ! TransLeq( o2, d2 ) )
		std::swap( o2, d2 );
	if ( ! TransLeq( o1, o2 ) )
	{
		std::swap( o1, o2 );
		std::swap( d1, d2 );
	}

	if ( ! TransLeq( o2, d1 ) )
	{
		// Technically, no intersection -- do our best
		v->set_y( ( o2->y() + d1->y() ) / 2.0 );
	}
	else if ( TransLeq( d1, d2 ) )
	{
		// Interpolate between o2 and d1
		z1 = TransEval( o1, o2, d1 );
		z2 = TransEval( o2, d1, d2 );
		if ( z1 + z2 < 0 )
		{
			z1 = -z1;
			z2 = -z2;
		}
		v->set_y( Interpolate( z1, o2->y(), z2, d1->y() ) );
	}
	else
	{
		// Interpolate between o2 and d2
		z1 = TransSign( o1, o2, d1 );
		z2 = -TransSign( o1, d2, d1 );
		if ( z1 + z2 < 0 )
		{
			z1 = -z1;
			z2 = -z2;
		}
		v->set_y( Interpolate( z1, o2->y(), z2, d2->y() ) );
	}
}

}
// END OF GEOM.H

// Splice( a, b ) is best described by the Guibas/Stolfi paper or the
// CS348a notes (see mesh.h).  Basically it modifies the mesh so that
// a->Onext and b->Onext are exchanged.  This can have various effects
// depending on whether a and b belong to different face or vertex rings.
// For more explanation see tessMeshSplice() below.
static void Splice( half_edge *a, half_edge *b )
{
	half_edge *aOnext = a->onext();
	half_edge *bOnext = b->onext();

	aOnext->lprev( b );
	bOnext->lprev( a );
	a->onext( bOnext );
	b->onext( aOnext );
}

// MakeVertex( newVertex, eOrig, vNext ) attaches a new vertex and makes it the
// origin of all edges in the vertex loop to which eOrig belongs. "vNext" gives
// a place to insert the new vertex in the global vertex list.  We insert
// the new vertex *before* vNext so that algorithms which walk the vertex
// list will not see the newly created vertices.
static void MakeVertex( vertex *newVertex, half_edge *eOrig, vertex *vNext )
{
	vertex *vNew = newVertex;

	assert( vNew != nullptr );

	// insert in circular doubly-linked list before vNext
	vertex *vPrev = vNext->prev();
	vNew->set_prev( vPrev );
	vPrev->set_next( vNew );
	vNew->set_next( vNext );
	vNext->set_prev( vNew );

	vNew->set_edge( eOrig );

	// leave s, t undefined

	// fix other edges on this vertex loop
	half_edge *e = eOrig;
	do
	{
		e->org( vNew );
		e = e->onext();
	} while ( e != eOrig );
}

// MakeFace( newFace, eOrig, fNext ) attaches a new face and makes it the left
// face of all edges in the face loop to which eOrig belongs.  "fNext" gives
// a place to insert the new face in the global face list.  We insert
// the new face *before* fNext so that algorithms which walk the face
// list will not see the newly created faces.
static void MakeFace( face *newFace, half_edge *eOrig, face *fNext )
{
	half_edge *e;
	face *fPrev;
	face *fNew = newFace;

	assert( fNew != nullptr );

	// insert in circular doubly-linked list before fNext
	fPrev = fNext->prev();
	fNew->set_prev( fPrev );
	fPrev->set_next( fNew );
	fNew->set_next( fNext );
	fNext->set_prev( fNew );

	fNew->set_edge( eOrig );
	fNew->set_trail( nullptr );
	fNew->set_marked( false );

	// The new face is marked "inside" if the old one was.  This is a
	// convenience for the common case where a face has been split in two.
	fNew->set_inside( fNext->inside() );

	// fix other edges on this face loop
	e = eOrig;
	do
	{
		e->lface( fNew );
		e = e->lnext();
	} while ( e != eOrig );
}

static size_t CountFaceVerts( face *f )
{
	half_edge *eCur = f->edge();
	size_t n = 0;
	do
	{
		n++;
		eCur = eCur->lnext();
	} while ( eCur != f->edge() );

	return n;
}


class tess_mesh
{
public:
	// tessMeshNewMesh() creates a new mesh with no edges, no vertices,
	// and no loops (what we usually call a "face").
	tess_mesh( void )
	{
		vertex *v = &_vertex_list;
		face *f = &_face_list;
		half_edge *e = &_edge_list;
		half_edge *eSym = &_edge_sym_list;

		v->set_next( v );
		v->set_prev( v );
		v->set_edge( nullptr );

		f->set_next( f );
		f->set_prev( f );
		f->set_edge( nullptr );
		f->set_trail( nullptr );
		f->set_marked( false );
		f->set_inside( false );

		e->next( e );
		e->sym(  eSym );

		eSym->next( eSym );
		eSym->sym( e );
	}

	// mesh::make_edge creates one edge, two vertices, and a loop (face).
	// The loop consists of the two new half-edges.
	half_edge *make_edge( void )
	{
		vertex *newVertex1 = _vertex_pool.allocate();
		vertex *newVertex2 = _vertex_pool.allocate();
		face *newFace = _face_pool.allocate();

		half_edge *e = make_edge( &_edge_list );

		MakeVertex( newVertex1, e, &_vertex_list );
		MakeVertex( newVertex2, e->sym(), &_vertex_list );
		MakeFace( newFace, e, &_face_list );
		return e;
	}

	// mesh::splice( eOrg, eDst ) is the basic operation for changing the
	// mesh connectivity and topology.  It changes the mesh so that
	//	eOrg->Onext <- OLD( eDst->Onext )
	//	eDst->Onext <- OLD( eOrg->Onext )
	// where OLD(...) means the value before the meshSplice operation.
	//
	// This can have two effects on the vertex structure:
	//  - if eOrg->Org != eDst->Org, the two vertices are merged together
	//  - if eOrg->Org == eDst->Org, the origin is split into two vertices
	// In both cases, eDst->Org is changed and eOrg->Org is untouched.
	//
	// Similarly (and independently) for the face structure,
	//  - if eOrg->Lface == eDst->Lface, one loop is split into two
	//  - if eOrg->Lface != eDst->Lface, two distinct loops are joined into one
	// In both cases, eDst->Lface is changed and eOrg->Lface is unaffected.
	//
	// Some special cases:
	// If eDst == eOrg, the operation has no effect.
	// If eDst == eOrg->Lnext, the new face will have a single edge.
	// If eDst == eOrg->Lprev, the old face will have a single edge.
	// If eDst == eOrg->Onext, the new vertex will have a single edge.
	// If eDst == eOrg->Oprev, the old vertex will have a single edge.
	void splice( half_edge *eOrg, half_edge *eDst )
	{
		bool joiningLoops = false;
		bool joiningVertices = false;

		if ( eOrg == eDst )
			return;

		if ( eDst->org() != eOrg->org() )
		{
			// We are merging two disjoint vertices -- destroy eDst->Org
			joiningVertices = true;
			kill_vertex( eDst->org(), eOrg->org() );
		}
		if ( eDst->lface() != eOrg->lface() )
		{
			// We are connecting two disjoint loops -- destroy eDst->Lface
			joiningLoops = true;
			kill_face( eDst->lface(), eOrg->lface() );
		}

		// Change the edge structure
		Splice( eDst, eOrg );

		if ( ! joiningVertices )
		{
			vertex *newVertex = _vertex_pool.allocate();

			// We split one vertex into two -- the new vertex is eDst->Org.
			// Make sure the old vertex points to a valid half-edge.
			MakeVertex( newVertex, eDst, eOrg->org() );
			eOrg->org()->set_edge( eOrg );
		}
		if ( ! joiningLoops )
		{
			face *newFace = _face_pool.allocate();

			// We split one loop into two -- the new loop is eDst->Lface.
			// Make sure the old face points to a valid half-edge.
			MakeFace( newFace, eDst, eOrg->lface() );
			eOrg->lface()->set_edge( eOrg );
		}
	}

	// @brief Removes the edge eDel.
	// There are several cases:
	// if eDel->lface() != eDel->rface(), we join two loops into one; the loop
	// eDel->lface() is deleted.  Otherwise, we are splitting one loop into two;
	// the newly created loop will contain eDel->dst().  If the deletion of eDel
	// would create isolated vertices, those are deleted as well.
	//
	// This function could be implemented as two calls to tessMeshSplice
	// plus a few calls to memFree, but this would allocate and delete
	// unnecessary vertices and faces.
	void delete_edge( half_edge *eDel )
	{
		half_edge *eDelSym = eDel->sym();
		bool joiningLoops = false;

		// First step: disconnect the origin vertex eDel->Org.  We make all
		// changes to get a consistent mesh in this "intermediate" state.
		if ( eDel->lface() != eDel->rface() )
		{
			// We are joining two loops into one -- remove the left face
			joiningLoops = true;
			kill_face( eDel->lface(), eDel->rface() );
		}

		if ( eDel->onext() == eDel )
			kill_vertex( eDel->org(), nullptr );
		else
		{
			// Make sure that eDel->Org and eDel->Rface point to valid half-edges
			eDel->rface()->set_edge( eDel->oprev() );
			eDel->org()->set_edge( eDel->onext() );

			Splice( eDel, eDel->oprev() );
			if ( ! joiningLoops )
			{
				face *newFace = _face_pool.allocate();

				// We are splitting one loop into two -- create a new loop for eDel.
				MakeFace( newFace, eDel, eDel->lface() );
			}
		}

		// Claim: the mesh is now in a consistent state, except that eDel->Org
		// may have been deleted.  Now we disconnect eDel->Dst.
		if ( eDelSym->onext() == eDelSym )
		{
			kill_vertex( eDelSym->org(), nullptr );
			kill_face( eDelSym->lface(), nullptr );
		}
		else
		{
			// Make sure that eDel->Dst and eDel->Lface point to valid half-edges
			eDel->lface()->set_edge( eDelSym->oprev() );
			eDelSym->org()->set_edge( eDelSym->onext() );
			Splice( eDelSym, eDelSym->oprev() );
		}

		// Any isolated vertices or faces have already been freed.
		kill_edge( eDel );
	}

	////////////////////////////////////////
	// Other Edge Operations
	// All these routines can be implemented with the basic edge
	// operations above.  They are provided for convenience and efficiency.
	////////////////////////////////////////

	// tessMeshAddEdgeVertex( eOrg ) creates a new edge eNew such that
	// eNew == eOrg->Lnext, and eNew->Dst is a newly created vertex.
	// eOrg and eNew will have the same left face.
	half_edge *add_edge_vertex( half_edge *eOrg )
	{
		half_edge *eNewSym;
		half_edge *eNew = make_edge( eOrg );
		if ( eNew == nullptr ) return nullptr;

		eNewSym = eNew->sym();

		// Connect the new edge appropriately
		Splice( eNew, eOrg->lnext() );

		// Set the vertex and face information
		eNew->org( eOrg->dst() );
		{
			vertex *newVertex = _vertex_pool.allocate();
			if ( newVertex == nullptr ) return nullptr;

			MakeVertex( newVertex, eNewSym, eNew->org() );
		}
		eNew->lface( eOrg->lface() );
		eNewSym->lface( eOrg->lface() );

		return eNew;
	}

	// tessMeshSplitEdge( eOrg ) splits eOrg into two edges eOrg and eNew,
	// such that eNew == eOrg->Lnext.  The new vertex is eOrg->Dst == eNew->Org.
	// eOrg and eNew will have the same left face.
	half_edge *split_edge( half_edge *eOrg )
	{
		half_edge *eNew;
		half_edge *tempHalfEdge = add_edge_vertex( eOrg );

		eNew = tempHalfEdge->sym();

		// Disconnect eOrg from eOrg->Dst and connect it to eNew->Org
		Splice( eOrg->sym(), eOrg->sym()->oprev() );
		Splice( eOrg->sym(), eNew );

		// Set the vertex and face information
		eOrg->dst( eNew->org() );
		eNew->dst()->set_edge( eNew->sym() ); // may have pointed to eOrg->Sym
		eNew->rface( eOrg->rface() );
		eNew->winding( eOrg->winding() ); // copy old winding information
		eNew->sym()->winding( eOrg->sym()->winding() );
		return eNew;
	}

	// tessMeshConnect( eOrg, eDst ) creates a new edge from eOrg->Dst
	// to eDst->Org, and returns the corresponding half-edge eNew.
	// If eOrg->Lface == eDst->Lface, this splits one loop into two,
	// and the newly created loop is eNew->Lface.  Otherwise, two disjoint
	// loops are merged into one, and the loop eDst->Lface is destroyed.
	//
	// If (eOrg == eDst), the new face will have only two edges.
	// If (eOrg->Lnext == eDst), the old face is reduced to a single edge.
	// If (eOrg->Lnext->Lnext == eDst), the old face is reduced to two edges.
	half_edge *connect( half_edge *eOrg, half_edge *eDst )
	{
		half_edge *eNewSym;
		bool joiningLoops = false;
		half_edge *eNew = make_edge( eOrg );
		if ( eNew == nullptr ) return nullptr;

		eNewSym = eNew->sym();

		if ( eDst->lface() != eOrg->lface() )
		{
			// We are connecting two disjoint loops -- destroy eDst->Lface
			joiningLoops = true;
			kill_face( eDst->lface(), eOrg->lface() );
		}

		// Connect the new edge appropriately
		Splice( eNew, eOrg->lnext() );
		Splice( eNewSym, eDst );

		// Set the vertex and face information
		eNew->org( eOrg->dst() );
		eNewSym->org( eDst->org() );
		eNew->lface( eOrg->lface() );
		eNewSym->lface( eOrg->lface() );

		// Make sure the old face points to a valid half-edge
		eOrg->lface()->set_edge( eNewSym );

		if ( ! joiningLoops )
		{
			face *newFace = _face_pool.allocate();
			if ( newFace == nullptr ) return nullptr;

			// We split one loop into two -- the new loop is eNew->Lface
			MakeFace( newFace, eNew, eOrg->lface() );
		}
		return eNew;
	}

	// mesh::check_mesh() checks a mesh for self-consistency.
	void check_mesh( void )
	{
		face *fH = &_face_list;
		vertex *vH = &_vertex_list;
		half_edge *eH = &_edge_list;
		face *f;
		vertex *v, *vPrev;

		face *fPrev = fH;
		for ( fPrev = fH; ( f = fPrev->next() ) != fH; fPrev = f )
		{
			assert( f->prev() == fPrev );
			half_edge *e = f->edge();
			do
			{
				assert( e->sym() != e );
				assert( e->sym()->sym() == e );
				assert( e->lnext()->onext()->sym() == e );
				assert( e->onext()->sym()->lnext() == e );
				assert( e->lface() == f );
				e = e->lnext();
			} while ( e != f->edge() );
		}
		assert( f->prev() == fPrev && f->edge() == nullptr );

		vPrev = vH;
		half_edge *e;
		for ( vPrev = vH ; ( v = vPrev->next() ) != vH; vPrev = v )
		{
			assert( v->prev() == vPrev );
			e = v->edge();
			do
			{
				assert( e->sym() != e );
				assert( e->sym()->sym() == e );
				assert( e->lnext()->onext()->sym() == e );
				assert( e->onext()->sym()->lnext() == e );
				assert( e->org() == v );
				e = e->onext();
			}
			while ( e != v->edge() );
		}
		assert( v->prev() == vPrev && v->edge() == nullptr );

		half_edge *ePrev = eH;
		for ( ePrev = eH; ( e = ePrev->next() ) != eH; ePrev = e )
		{
			assert( e->sym()->next() == ePrev->sym() );
			assert( e->sym() != e );
			assert( e->sym()->sym() == e );
			assert( e->org() != nullptr );
			assert( e->dst() != nullptr );
			assert( e->lnext()->onext()->sym() == e );
			assert( e->onext()->sym()->lnext() == e );
		}
		assert( e->sym()->next() == ePrev->sym()
				&& e->sym() == &_edge_sym_list
				&& e->sym()->sym() == e
				&& e->org() == nullptr && e->dst() == nullptr
				&& e->lface() == nullptr && e->rface() == nullptr );
	}

	int merge_convex_faces( size_t maxVertsPerFace )
	{
		face *f;
		half_edge *eCur, *eNext, *eSym;
		vertex *vStart;
		size_t curNv, symNv;

		for ( f = _face_list.next(); f != &_face_list; f = f->next() )
		{
			// Skip faces which are outside the result.
			if ( !f->inside() )
				continue;

			eCur = f->edge();
			vStart = eCur->org();

			while ( 1 )
			{
				eNext = eCur->lnext();
				eSym = eCur->sym();

				// Try to merge if the neighbour face is valid.
				if ( eSym && eSym->lface() && eSym->lface()->inside() )
				{
					// Try to merge the neighbour faces if the resulting polygons
					// does not exceed maximum number of vertices.
					curNv = CountFaceVerts( f );
					symNv = CountFaceVerts( eSym->lface() );
					if ( ( curNv + symNv - 2 ) <= maxVertsPerFace )
					{
						// Merge if the resulting poly is convex.
						if ( VertCCW( eCur->lprev()->org(), eCur->org(), eSym->lnext()->lnext()->org() ) &&
								VertCCW( eSym->lprev()->org(), eSym->org(), eCur->lnext()->lnext()->org() ) )
						{
							eNext = eSym->lnext();
							delete_edge( eSym );
							eCur = 0;
						}
					}
				}

				if ( eCur && eCur->lnext()->org() == vStart )
					break;

				// Continue to next edge.
				eCur = eNext;
			}
		}

		return 1;
	}

	// Remove zero-length edges, and contours with fewer than 3 vertices.
	void remove_degenerate_edges( void )
	{
		half_edge *eH = &_edge_list;
		half_edge *eNext;

		for ( half_edge *e = eH->next(); e != eH; e = eNext )
		{
			eNext = e->next();
			half_edge *eLnext = e->lnext();

			if ( VertEq( e->org(), e->dst() ) && e->lnext()->lnext() != e )
			{
				// Zero-length edge, contour has at least 3 edges

				splice( eLnext, e );
				delete_edge( e );
				e = eLnext;
				eLnext = e->lnext();
			}
			if ( eLnext->lnext() == e )
			{
				// Degenerate contour (one or two edges)

				if ( eLnext != e )
				{
					if ( eLnext == eNext || eLnext == eNext->sym() )
						eNext = eNext->next();
					delete_edge( eLnext );
				}
				if ( e == eNext || e == eNext->sym() )
					eNext = eNext->next();
				delete_edge( e );
			}
		}
	}

	// tessMeshTessellateInterior( mesh ) tessellates each region of
	// the mesh which is marked "inside" the polygon.  Each such region
	// must be monotone.
	bool tessellate_interior( void )
	{
		face *next;
		for ( face *f = _face_list.next(); f != &_face_list; f = next )
		{
			// Make sure we don''t try to tessellate the new triangles.
			next = f->next();
			if ( f->inside() )
			{
				if ( !tessellate_mono_region( f ) )
					return false;
			}
		}

		return true;
	}

	// tessMeshTessellateMonoRegion( face ) tessellates a monotone region
	// (what else would it do??)  The region must consist of a single
	// loop of half-edges (see mesh.h) oriented CCW.  "Monotone" in this
	// case means that any vertical line intersects the interior of the
	// region in a single interval.
	//
	// Tessellation consists of adding interior edges (actually pairs of
	// half-edges), to split the region into non-overlapping triangles.
	//
	// The basic idea is explained in Preparata and Shamos (which I don''t
	// have handy right now), although their implementation is more
	// complicated than this one.  The are two edge chains, an upper chain
	// and a lower chain.  We process all vertices from both chains in order,
	// from right to left.
	//
	// The algorithm ensures that the following invariant holds after each
	// vertex is processed: the untessellated region consists of two
	// chains, where one chain (say the upper) is a single edge, and
	// the other chain is concave.  The left vertex of the single edge
	// is always to the left of all vertices in the concave chain.
	//
	// Each step consists of adding the rightmost unprocessed vertex to one
	// of the two chains, and forming a fan of triangles from the rightmost
	// of two chain endpoints.  Determining whether we can add each triangle
	// to the fan is a simple orientation test.  By making the fan as large
	// as possible, we restore the invariant (check it yourself).
	int tessellate_mono_region( face *f )
	{
		half_edge *up, *lo;

		// All edges are oriented CCW around the boundary of the region.
		// First, find the half-edge whose origin vertex is rightmost.
		// Since the sweep goes from left to right, f->anEdge should
		// be close to the edge we want.
		up = f->edge();
		assert( up->lnext() != up && up->lnext()->lnext() != up );

		for ( ; VertLeq( up->dst(), up->org() ); up = up->lprev() )
			;
		for ( ; VertLeq( up->org(), up->dst() ); up = up->lnext() )
			;
		lo = up->lprev();

		while ( up->lnext() != lo )
		{
			if ( VertLeq( up->dst(), lo->org() ) )
			{
				// up->Dst is on the left.  It is safe to form triangles from lo->Org.
				// The EdgeGoesLeft test guarantees progress even when some triangles
				// are CW, given that the upper and lower chains are truly monotone.
				while ( lo->lnext() != up && ( EdgeGoesLeft( lo->lnext() )
											 || EdgeSign( lo->org(), lo->dst(), lo->lnext()->dst() ) <= 0 ) )
				{
					half_edge *tempHalfEdge = connect( lo->lnext(), lo );
					if ( tempHalfEdge == nullptr ) return 0;
					lo = tempHalfEdge->sym();
				}
				lo = lo->lprev();
			}
			else
			{
				// lo->Org is on the left.  We can make CCW triangles from up->Dst.
				while ( lo->lnext() != up && ( EdgeGoesRight( up->lprev() )
											 || EdgeSign( up->dst(), up->org(), up->lprev()->org() ) >= 0 ) )
				{
					half_edge *tempHalfEdge = connect( up, up->lprev() );
					if ( tempHalfEdge == nullptr ) return 0;
					up = tempHalfEdge->sym();
				}
				up = up->lnext();
			}
		}

		// Now lo->Org == up->Dst == the leftmost vertex.  The remaining region
		// can be tessellated in a fan from this leftmost vertex.
		assert( lo->lnext() != up );
		while ( lo->lnext()->lnext() != up )
		{
			half_edge *tempHalfEdge = connect( lo->lnext(), lo );
			if ( tempHalfEdge == nullptr ) return 0;
			lo = tempHalfEdge->sym();
		}

		return 1;
	}

	// Delete any degenerate faces with only two edges.  WalkDirtyRegions()
	// will catch almost all of these, but it won't catch degenerate faces
	// produced by splice operations on already-processed edges.
	// The two places this can happen are in FinishLeftRegions(), when
	// we splice in a "temporary" edge produced by ConnectRightVertex(),
	// and in CheckForLeftSplice(), where we splice already-processed
	// edges to ensure that our dictionary invariants are not violated
	// by numerical errors.
	//
	// In both these cases it is *very* dangerous to delete the offending
	// edge at the time, since one of the routines further up the stack
	// will sometimes be keeping a pointer to that edge.
	int remove_degenerate_faces( void )
	{
		face *f, *fNext;

		for ( f = _face_list.next(); f != &_face_list; f = fNext )
		{
			fNext = f->next();
			half_edge *e = f->edge();
			assert( e->lnext() != e );

			if ( e->lnext()->lnext() == e )
			{
				// A face with only two edges
				half_edge *on = e->onext();
				on->add_winding( e->winding() );
				on->sym()->add_winding( e->sym()->winding() );
				delete_edge( e );
			}
		}
		return 1;
	}

	// KillEdge( eDel ) destroys an edge (the half-edges eDel and eDel->Sym),
	// and removes from the global edge list.
	void kill_edge( half_edge *eDel )
	{
		half_edge *ePrev, *eNext;

		// delete from circular doubly-linked list
		eNext = eDel->next();
		ePrev = eDel->sym()->next();
		eNext->sym()->next( ePrev );
		ePrev->sym()->next( eNext );

		_edge_pool.deallocate( eDel->sym() );
		_edge_pool.deallocate( eDel );
	}

	// KillVertex( vDel ) destroys a vertex and removes it from the global
	// vertex list.  It updates the vertex loop to point to a given new vertex.
	void kill_vertex( vertex *vDel, vertex *newOrg )
	{
		half_edge *e, *eStart = vDel->edge();
		vertex *vPrev, *vNext;

		// change the origin of all affected edges
		e = eStart;
		do
		{
			e->org( newOrg );
			e = e->onext();
		}
		while ( e != eStart );

		// delete from circular doubly-linked list
		vPrev = vDel->prev();
		vNext = vDel->next();
		vNext->set_prev( vPrev );
		vPrev->set_next( vNext );

		_vertex_pool.deallocate( vDel );
	}

	// KillFace( fDel ) destroys a face and removes it from the global face
	// list.  It updates the face loop to point to a given new face.
	void kill_face( face *fDel, face *newLface )
	{
		half_edge *e, *eStart = fDel->edge();
		face *fPrev, *fNext;

		// change the left face of all affected edges
		e = eStart;
		do
		{
			e->lface( newLface );
			e = e->lnext();
		} while ( e != eStart );

		// delete from circular doubly-linked list
		fPrev = fDel->prev();
		fNext = fDel->next();
		fNext->set_prev( fPrev );
		fPrev->set_next( fNext );

		_face_pool.deallocate( fDel );
	}

	face *face_list( void )
	{
		return &_face_list;
	}

	vertex *vertex_list( void )
	{
		return &_vertex_list;
	}

private:
	// MakeEdge creates a new pair of half-edges which form their own loop.
	// No vertex or face structures are allocated, but these must be assigned
	// before the current edge operation is completed.
	half_edge *make_edge( half_edge *eNext )
	{
		half_edge *e = _edge_pool.allocate();
		half_edge *eSym = _edge_pool.allocate();

		// Make sure eNext points to the first edge of the edge pair
		if ( eNext->sym() < eNext )
			eNext = eNext->sym();

		// Insert in circular doubly-linked list before eNext.
		// Note that the prev pointer is stored in Sym->next.
		half_edge *ePrev = eNext->prev();
		eSym->next( ePrev );
		ePrev->prev( e );
		e->next( eNext );
		eNext->prev( eSym );

		e->sym( eSym );
		e->onext( e );
		e->lnext( eSym );

		eSym->sym( e );
		eSym->onext( eSym );
		eSym->lnext( e );

		return e;
	}

	vertex _vertex_list; // dummy header for vertex list
	face _face_list; // dummy header for face list
	half_edge _edge_list; // dummy header for edge list
	half_edge _edge_sym_list; // and its symmetric counterpart

	base::memory_pool<half_edge,512> _edge_pool;
	base::memory_pool<vertex,512> _vertex_pool;
	base::memory_pool<face,256> _face_pool;
};

// The mesh operations below have three motivations: completeness,
// convenience, and efficiency.  The basic mesh operations are MakeEdge,
// Splice, and Delete.  All the other edge operations can be implemented
// in terms of these.  The other operations are provided for convenience
// and/or efficiency.
//
// When a face is split or a vertex is added, they are inserted into the
// global list *before* the existing vertex or face (ie. e->Org or e->Lface).
// This makes it easier to process all vertices or faces in the global lists
// without worrying about processing the same data twice.  As a convenience,
// when a face is split, the "inside" flag is copied from the old face.
// Other internal data (v->data, v->activeRegion, f->data, f->marked,
// f->trail, e->winding) is set to zero.
//
// ********************** Basic Edge Operations **************************
//
// tessMeshMakeEdge( mesh ) creates one edge, two vertices, and a loop.
// The loop (face) consists of the two new half-edges.
//
// tessMeshSplice( eOrg, eDst ) is the basic operation for changing the
// mesh connectivity and topology.  It changes the mesh so that
//  eOrg->Onext <- OLD( eDst->Onext )
//  eDst->Onext <- OLD( eOrg->Onext )
// where OLD(...) means the value before the meshSplice operation.
//
// This can have two effects on the vertex structure:
//  - if eOrg->Org != eDst->Org, the two vertices are merged together
//  - if eOrg->Org == eDst->Org, the origin is split into two vertices
// In both cases, eDst->Org is changed and eOrg->Org is untouched.
//
// Similarly (and independently) for the face structure,
//  - if eOrg->Lface == eDst->Lface, one loop is split into two
//  - if eOrg->Lface != eDst->Lface, two distinct loops are joined into one
// In both cases, eDst->Lface is changed and eOrg->Lface is unaffected.
//
// tessMeshDelete( eDel ) removes the edge eDel.  There are several cases:
// if (eDel->Lface != eDel->Rface), we join two loops into one; the loop
// eDel->Lface is deleted.  Otherwise, we are splitting one loop into two;
// the newly created loop will contain eDel->Dst.  If the deletion of eDel
// would create isolated vertices, those are deleted as well.
//
// ********************** Other Edge Operations **************************
//
// tessMeshAddEdgeVertex( eOrg ) creates a new edge eNew such that
// eNew == eOrg->Lnext, and eNew->Dst is a newly created vertex.
// eOrg and eNew will have the same left face.
//
// tessMeshSplitEdge( eOrg ) splits eOrg into two edges eOrg and eNew,
// such that eNew == eOrg->Lnext.  The new vertex is eOrg->Dst == eNew->Org.
// eOrg and eNew will have the same left face.
//
// tessMeshConnect( eOrg, eDst ) creates a new edge from eOrg->Dst
// to eDst->Org, and returns the corresponding half-edge eNew.
// If eOrg->Lface == eDst->Lface, this splits one loop into two,
// and the newly created loop is eNew->Lface.  Otherwise, two disjoint
// loops are merged into one, and the loop eDst->Lface is destroyed.
//
// ************************ Other Operations *****************************
//
// tessMeshNewMesh() creates a new mesh with no edges, no vertices,
// and no loops (what we usually call a "face").
//
// tessMeshUnion( mesh1, mesh2 ) forms the union of all structures in
// both meshes, and returns the new mesh (the old meshes are destroyed).
//
// tessMeshDeleteMesh( mesh ) will free all storage for any valid mesh.
//
// tessMeshZapFace( fZap ) destroys a face and removes it from the
// global face list.  All edges of fZap will have a nullptr pointer as their
// left face.  Any edges which also have a nullptr pointer as their right face
// are deleted entirely (along with any isolated vertices this produces).
// An entire mesh can be deleted by zapping its faces, one at a time,
// in any order.  Zapped faces cannot be used in further mesh operations!
//
// tessMeshCheckMesh( mesh ) checks a mesh for self-consistency.
// END OF MESH.H

// For each pair of adjacent edges crossing the sweep line, there is
// an active_region to represent the region between them.  The active
// regions are kept in sorted order in a dynamic dictionary.  As the
// sweep line crosses each vertex, we update the affected regions.
class active_region
{
public:
	active_region *below( void )
	{
		return nodeUp->prev()->key();
	}

	active_region *above( void )
	{
		return nodeUp->next()->key();
	}

	half_edge *eUp; // upper edge, directed right to left
	dict<active_region*>::node *nodeUp;	// dictionary node corresponding to eUp
	int windingNumber; // used to determine which regions are inside the polygon
	bool inside; // is this region inside the polygon?
	bool sentinel; // marks fake edges at t = +/-infinity
	bool dirty;	// marks regions where the upper or lower edge has changed, but we haven't checked whether they intersect yet
	bool fixUpperEdge; // marks temporary edges introduced when we process a "right vertex" (one without any edges leaving to the right)
};

inline bool tesvertLeq( vertex *u, vertex *v )
{
	// Returns true if u is lexicographically <= v.
	return VertLeq( u, v );
}

// Find some weights which describe how the intersection vertex is
// a linear combination of "org" and "dest".  Each of the two edges
// which generated "isect" is allocated 50% of the weight; each edge
// splits the weight between its org and dst according to the
// relative distance to "isect".
static void VertexWeights( vertex *isect, vertex *org, vertex *dst, double *weights )
{
	double t1 = VertL1dist( org, isect );
	double t2 = VertL1dist( dst, isect );

	weights[0] = 0.5 * t2 / ( t1 + t2 );
	weights[1] = 0.5 * t1 / ( t1 + t2 );
	isect->set_x( isect->x() + weights[0] * org->x() + weights[1] * dst->x() );
	isect->set_y( isect->y() + weights[0] * org->y() + weights[1] * dst->y() );
}


// Determine the polygon normal and project vertices onto the plane
// of the polygon.
void tessellator::compute_bounding_box( void )
{
	vertex *vHead = _mesh->vertex_list();
	bool first = true;

	for ( vertex *v = vHead->next(); v != vHead; v = v->next() )
	{
		if ( first )
		{
			_bmin[0] = _bmax[0] = v->x();
			_bmin[1] = _bmax[1] = v->y();
			first = false;
		}
		else
		{
			if ( v->x() < _bmin[0] ) _bmin[0] = v->x();
			if ( v->x() > _bmax[0] ) _bmax[0] = v->x();
			if ( v->y() < _bmin[1] ) _bmin[1] = v->y();
			if ( v->y() > _bmax[1] ) _bmax[1] = v->y();
		}
	}
}

namespace {
	bool greater_than( const vertex *a, const vertex *b )
	{
		// Swap a & b to get greater than
		return !VertLeq( a, b );
	}
}

tessellator::tessellator( const std::function<void(double,double)> &add_point, const std::function<void(size_t,size_t,size_t)> &add_tri )
	: _pq( &greater_than ), _add_point( add_point ), _add_tri( add_tri )
{
	_pq.set_default( nullptr );

	// Only initialize fields which can be changed by the api.  Other fields
	// are initialized where they are used.
	_bmin[0] = 0;
	_bmin[1] = 0;
	_bmax[0] = 0;
	_bmax[1] = 0;

	// Initialize to begin polygon.
	_mesh = nullptr;

	_dict = nullptr;
	_event = nullptr;
}

tessellator::~tessellator( void )
{
	delete _mesh;
	_mesh = nullptr;
}

void tessellator::output_polymesh( void )
{
	size_t maxFaceCount = 0;
	size_t maxVertexCount = 0;

	// Mark unused
	for ( vertex *v = _mesh->vertex_list()->next(); v != _mesh->vertex_list(); v = v->next() )
		v->set_index( TESS_UNDEF );

	// Create unique IDs for all vertices and faces.
	for ( face *f = _mesh->face_list()->next(); f != _mesh->face_list(); f = f->next() )
	{
		f->set_index( TESS_UNDEF );
		if ( !f->inside() )
			continue;

		half_edge *edge = f->edge();
		size_t faceVerts = 0;
		do
		{
			vertex *v = edge->org();
			if ( v->index() == TESS_UNDEF )
			{
				v->set_index( maxVertexCount );
				maxVertexCount++;
			}
			faceVerts++;
			edge = edge->lnext();
		} while ( edge != f->edge() );

		assert( faceVerts <= 3 );

		f->set_index( maxFaceCount );
		++maxFaceCount;
	}

	std::vector<size_t> indexmap;
	indexmap.resize( maxVertexCount );

	// Output vertices.
	size_t count = 0;
	for ( vertex *v = _mesh->vertex_list()->next(); v != _mesh->vertex_list(); v = v->next() )
	{
		if ( v->index() != TESS_UNDEF )
		{
			// Store coordinate
			_add_point( v->x(), v->y() );
			indexmap[v->index()] = count;
			++count;
		}
	}

	// Output indices.
	for ( face *f = _mesh->face_list()->next(); f != _mesh->face_list(); f = f->next() )
	{
		if ( !f->inside() )
			continue;

		// Store polygon
		half_edge *edge = f->edge();

		vertex *v1 = edge->org();
		edge = edge->lnext();
		logic_check( edge != f->edge(), "not a triangle" );

		vertex *v2 = edge->org();
		edge = edge->lnext();
		logic_check( edge != f->edge(), "not a triangle" );

		vertex *v3 = edge->org();
		edge = edge->lnext();
		logic_check( edge == f->edge(), "not a triangle" );

		_add_tri( indexmap[v1->index()], indexmap[v2->index()], indexmap[v3->index()] );
	}
}

void *tessellator::begin_contour( void )
{
	if ( _mesh == nullptr )
		_mesh = new tess_mesh();
	return nullptr;
}

void tessellator::contour_point( void *&contour, double x, double y )
{
	half_edge *e = reinterpret_cast<half_edge *>( contour );
	bool first = ( e == nullptr );

	if ( first )
	{
		// Make a self-loop (one vertex, one edge).
		e = _mesh->make_edge();
		_mesh->splice( e, e->sym() );
	}
	else
	{
		// Create a new vertex and edge which immediately follow e
		// in the ordering around the left face.
		_mesh->split_edge( e );
		e = e->lnext();
	}

	// The new vertex is now e->Org.
	vertex *v = e->org();
	v->set( x, y );

	// The winding of an edge says how the winding number changes as we
	// cross from the edge''s right face to its left face.  We add the
	// vertices in such an order that a CCW contour will add +1 to
	// the winding number of the region inside the contour.
	e->winding( 1 );
	e->sym()->winding( -1 );

	contour = e;
}

void tessellator::end_contour( void *& )
{
}

void tessellator::tessellate( void )
{
	precondition( _mesh, "no contours added to tessellate" );

	// Determine the polygon normal and project vertices onto the plane of the polygon.
	compute_bounding_box();

	// tessComputeInterior( tess ) computes the planar arrangement specified
	// by the given contours, and further subdivides this arrangement
	// into regions.  Each region is marked "inside" if it belongs
	// to the polygon, according to the rule given by tess->windingRule.
	// Each interior region is guaranteed be monotone.
	if ( !compute_interior() )
		throw std::runtime_error( "interior computation failed" );

	// If the user wants only the boundary contours, we throw away all edges
	// except those which separate the interior from the exterior.
	// Otherwise we tessellate all the regions marked "inside".
	if ( !_mesh->tessellate_interior() )
		throw std::runtime_error( "interior tessellation failed" );

	_mesh->check_mesh();

	output_polymesh();

	delete _mesh;
	_mesh = nullptr;
}

// Both edges must be directed from right to left (this is the canonical
// direction for the upper edge of each region).
//
// The strategy is to evaluate a "t" value for each edge at the
// current sweep line position, given by tess->event.  The calculations
// are designed to be very stable, but of course they are not perfect.
//
// Special case: if both edge destinations are at the sweep event,
// we sort the edges by slope (they would otherwise compare equally).
bool tessellator::edge_leq( active_region *reg1, active_region *reg2 )
{
	half_edge *e1, *e2;
	double t1, t2;

	e1 = reg1->eUp;
	e2 = reg2->eUp;

	if ( e1->dst() == _event )
	{
		if ( e2->dst() == _event )
		{
			// Two edges right of the sweep line which meet at the sweep event.
			// Sort them by slope.
			if ( VertLeq( e1->org(), e2->org() ) )
				return EdgeSign( e2->dst(), e1->org(), e2->org() ) <= 0;
			return EdgeSign( e1->dst(), e2->org(), e1->org() ) >= 0;
		}
		return EdgeSign( e2->dst(), _event, e2->org() ) <= 0;
	}
	if ( e2->dst() == _event )
		return EdgeSign( e1->dst(), _event, e1->org() ) >= 0;

	// General case - compute signed distance *from* e1, e2 to event
	t1 = EdgeEval( e1->dst(), _event, e1->org() );
	t2 = EdgeEval( e2->dst(), _event, e2->org() );
	return ( t1 >= t2 );
}

void tessellator::delete_region( active_region *reg )
{
	if ( reg->fixUpperEdge )
	{
		// It was created with zero winding number, so it better be
		// deleted with zero winding number (ie. it better not get merged
		// with a real edge).
		assert( reg->eUp->winding() == 0 );
	}
	reg->eUp->region( nullptr );
	_dict->erase( reg->nodeUp );
	_regionPool.deallocate( reg );
}


// Replace an upper edge which needs fixing (see ConnectRightVertex).
int tessellator::fix_upper_edge( active_region *reg, half_edge *newEdge )
{
	assert( reg->fixUpperEdge );
	_mesh->delete_edge( reg->eUp );
	reg->fixUpperEdge = false;
	reg->eUp = newEdge;
	newEdge->region( reg );

	return 1;
}

active_region *tessellator::top_left_region( active_region *reg )
{
	vertex *org = reg->eUp->org();

	// Find the region above the uppermost edge with the same origin
	do
	{
		reg = reg->above();
	}
	while ( reg->eUp->org() == org );

	// If the edge above was a temporary edge introduced by ConnectRightVertex,
	// now is the time to fix it.
	if ( reg->fixUpperEdge )
	{
		half_edge *e = _mesh->connect( reg->below()->eUp->sym(), reg->eUp->lnext() );
		if ( e == nullptr )
			return nullptr;
		if ( !fix_upper_edge( reg, e ) )
			return nullptr;
		reg = reg->above();
	}
	return reg;
}

active_region *tessellator::top_right_region( active_region *reg )
{
	vertex *dst = reg->eUp->dst();

	// Find the region above the uppermost edge with the same destination
	do
	{
		reg = reg->above();
	}
	while ( reg->eUp->dst() == dst );
	return reg;
}

// Add a new active region to the sweep line, *somewhere* below "regAbove"
// (according to where the new edge belongs in the sweep-line dictionary).
// The upper edge of the new region will be "eNewUp".
// Winding number and "inside" flag are not updated.
active_region *tessellator::add_region_below( active_region *regAbove, half_edge *eNewUp )
{
	active_region *regNew = _regionPool.allocate();
	if ( regNew == nullptr )
		throw std::runtime_error( "alloc failed" );

	regNew->eUp = eNewUp;
	regNew->nodeUp = _dict->insert_before( regAbove->nodeUp, regNew );
	if ( regNew->nodeUp == nullptr )
		throw std::runtime_error( "dictionary insert failed" );
	regNew->fixUpperEdge = false;
	regNew->sentinel = false;
	regNew->dirty = false;

	eNewUp->region( regNew );
	return regNew;
}

void tessellator::compute_winding( active_region *reg )
{
	reg->windingNumber = reg->above()->windingNumber + reg->eUp->winding();
	reg->inside = is_winding_inside( reg->windingNumber );
}


// Delete a region from the sweep line.  This happens when the upper
// and lower chains of a region meet (at a vertex on the sweep line).
// The "inside" flag is copied to the appropriate mesh face (we could
// not do this before -- since the structure of the mesh is always
// changing, this face may not have even existed until now).
void tessellator::finish_region( active_region *reg )
{
	half_edge *e = reg->eUp;
	face *f = e->lface();

	f->set_inside( reg->inside );
	f->set_edge( e ); // optimization for tessMeshTessellateMonoRegion()
	delete_region( reg );
}


// We are given a vertex with one or more left-going edges.  All affected
// edges should be in the edge dictionary.  Starting at regFirst->eUp,
// we walk down deleting all regions where both edges have the same
// origin vOrg.  At the same time we copy the "inside" flag from the
// active region to the face, since at this point each face will belong
// to at most one region (this was not necessarily true until this point
// in the sweep).  The walk stops at the region above regLast; if regLast
// is nullptr we walk as far as possible.  At the same time we relink the
// mesh if necessary, so that the ordering of edges around vOrg is the
// same as in the dictionary.
half_edge *tessellator::finish_left_regions( active_region *regFirst, active_region *regLast )
{
	active_region *regPrev = regFirst;
	half_edge *ePrev = regFirst->eUp;
	while ( regPrev != regLast )
	{
		regPrev->fixUpperEdge = false; // placement was OK
		active_region *reg = regPrev->below();
		half_edge *e = reg->eUp;
		if ( e->org() != ePrev->org() )
		{
			if ( ! reg->fixUpperEdge )
			{
				// Remove the last left-going edge.  Even though there are no further
				// edges in the dictionary with this origin, there may be further
				// such edges in the mesh (if we are adding left edges to a vertex
				// that has already been processed).  Thus it is important to call
				// FinishRegion rather than just DeleteRegion.
				finish_region( regPrev );
				break;
			}
			// If the edge below was a temporary edge introduced by
			// ConnectRightVertex, now is the time to fix it.
			e = _mesh->connect( ePrev->lprev(), e->sym() );
			if ( e == nullptr )
				throw std::runtime_error( "mesh connect failed" );
			if ( !fix_upper_edge( reg, e ) )
				throw std::runtime_error( "couldn't fix upper edge" );
		}

		// Relink edges so that ePrev->Onext == e
		if ( ePrev->onext() != e )
		{
			_mesh->splice( e->oprev(), e );
			_mesh->splice( ePrev, e );
		}
		finish_region( regPrev ); // may change reg->eUp
		ePrev = reg->eUp;
		regPrev = reg;
	}
	return ePrev;
}

inline void AddWinding( half_edge *eDst, half_edge *eSrc )
{
	eDst->add_winding( eSrc->winding() );
	eDst->sym()->add_winding( eSrc->sym()->winding() );
}

// Purpose: insert right-going edges into the edge dictionary, and update
// winding numbers and mesh connectivity appropriately.  All right-going
// edges share a common origin vOrg.  Edges are inserted CCW starting at
// eFirst; the last edge inserted is eLast->Oprev.  If vOrg has any
// left-going edges already processed, then eTopLeft must be the edge
// such that an imaginary upward vertical segment from vOrg would be
// contained between eTopLeft->Oprev and eTopLeft; otherwise eTopLeft
// should be nullptr.
void tessellator::add_right_edges( active_region *regUp, half_edge *eFirst, half_edge *eLast, half_edge *eTopLeft, bool cleanUp )
{
	active_region *reg, *regPrev;
	half_edge *e, *ePrev;
	bool firstTime = true;

	// Insert the new right-going edges in the dictionary
	e = eFirst;
	do
	{
		assert( VertLeq( e->org(), e->dst() ) );
		add_region_below( regUp, e->sym() );
		e = e->onext();
	}
	while ( e != eLast );

	// Walk *all* right-going edges from e->Org, in the dictionary order,
	// updating the winding numbers of each region, and re-linking the mesh
	// edges to match the dictionary ordering (if necessary).
	if ( eTopLeft == nullptr )
		eTopLeft = regUp->below()->eUp->rprev();
	regPrev = regUp;
	ePrev = eTopLeft;
	for ( ;; )
	{
		reg = regPrev->below();
		e = reg->eUp->sym();
		if ( e->org() != ePrev->org() ) break;

		if ( e->onext() != ePrev )
		{
			// Unlink e from its current position, and relink below ePrev
			_mesh->splice( e->oprev(), e );
			_mesh->splice( ePrev->oprev(), e );
		}
		// Compute the winding number and "inside" flag for the new regions
		reg->windingNumber = regPrev->windingNumber - e->winding();
		reg->inside = is_winding_inside( reg->windingNumber );

		// Check for two outgoing edges with same slope -- process these
		// before any intersection tests (see example in tessComputeInterior).
		regPrev->dirty = true;
		if ( ! firstTime && check_for_right_splice( regPrev ) )
		{
			AddWinding( e, ePrev );
			delete_region( regPrev );
			_mesh->delete_edge( ePrev );
		}
		firstTime = false;
		regPrev = reg;
		ePrev = e;
	}
	regPrev->dirty = true;
	assert( regPrev->windingNumber - e->winding() == reg->windingNumber );

	if ( cleanUp )
	{
		// Check for intersections between newly adjacent edges.
		walk_dirty_regions( regPrev );
	}
}

// Check the upper and lower edge of "regUp", to make sure that the
// eUp->Org is above eLo, or eLo->Org is below eUp (depending on which
// origin is leftmost).
//
// The main purpose is to splice right-going edges with the same
// dest vertex and nearly identical slopes (ie. we can't distinguish
// the slopes numerically).  However the splicing can also help us
// to recover from numerical errors.  For example, suppose at one
// point we checked eUp and eLo, and decided that eUp->Org is barely
// above eLo.  Then later, we split eLo into two edges (eg. from
// a splice operation like this one).  This can change the result of
// our test so that now eUp->Org is incident to eLo, or barely below it.
// We must correct this condition to maintain the dictionary invariants.
//
// One possibility is to check these edges for intersection again
// (ie. CheckForIntersect).  This is what we do if possible.  However
// CheckForIntersect requires that tess->event lies between eUp and eLo,
// so that it has something to fall back on when the intersection
// calculation gives us an unusable answer.  So, for those cases where
// we can't check for intersection, this routine fixes the problem
// by just splicing the offending vertex into the other edge.
// This is a guaranteed solution, no matter how degenerate things get.
// Basically this is a combinatorial solution to a numerical problem.
bool tessellator::check_for_right_splice( active_region *regUp )
{
	active_region *regLo = regUp->below();
	half_edge *eUp = regUp->eUp;
	half_edge *eLo = regLo->eUp;

	if ( VertLeq( eUp->org(), eLo->org() ) )
	{
		if ( EdgeSign( eLo->dst(), eUp->org(), eLo->org() ) > 0 ) return false;

		// eUp->Org appears to be below eLo
		if ( ! VertEq( eUp->org(), eLo->org() ) )
		{
			// Splice eUp->Org into eLo
			_mesh->split_edge( eLo->sym() );
			_mesh->splice( eUp, eLo->oprev() );
			regUp->dirty = regLo->dirty = true;

		}
		else if ( eUp->org() != eLo->org() )
		{
			// merge the two vertices, discarding eUp->Org
			_pq.erase( eUp->org() );
			_mesh->splice( eLo->oprev(), eUp );
		}
	}
	else
	{
		if ( EdgeSign( eUp->dst(), eLo->org(), eUp->org() ) < 0 ) return false;

		// eLo->Org appears to be above eUp, so splice eLo->Org into eUp
		regUp->above()->dirty = regUp->dirty = true;
		_mesh->split_edge( eUp->sym() );
		_mesh->splice( eLo->oprev(), eUp );
	}
	return true;
}

// Check the upper and lower edge of "regUp", to make sure that the
// eUp->Dst is above eLo, or eLo->Dst is below eUp (depending on which
// destination is rightmost).
//
// Theoretically, this should always be true.  However, splitting an edge
// into two pieces can change the results of previous tests.  For example,
// suppose at one point we checked eUp and eLo, and decided that eUp->Dst
// is barely above eLo.  Then later, we split eLo into two edges (eg. from
// a splice operation like this one).  This can change the result of
// the test so that now eUp->Dst is incident to eLo, or barely below it.
// We must correct this condition to maintain the dictionary invariants
// (otherwise new edges might get inserted in the wrong place in the
// dictionary, and bad stuff will happen).
//
// We fix the problem by just splicing the offending vertex into the
// other edge.
bool tessellator::check_for_left_splice( active_region *regUp )
{
	active_region *regLo = regUp->below();
	half_edge *eUp = regUp->eUp;
	half_edge *eLo = regLo->eUp;
	half_edge *e;

	assert( ! VertEq( eUp->dst(), eLo->dst() ) );

	if ( VertLeq( eUp->dst(), eLo->dst() ) )
	{
		if ( EdgeSign( eUp->dst(), eLo->dst(), eUp->org() ) < 0 ) return false;

		// eLo->Dst is above eUp, so splice eLo->Dst into eUp
		regUp->above()->dirty = regUp->dirty = true;
		e = _mesh->split_edge( eUp );
		if ( e == nullptr )
			throw std::runtime_error( "mesh edge split failed" );
		_mesh->splice( eLo->sym(), e );
		e->lface()->set_inside( regUp->inside );
	}
	else
	{
		if ( EdgeSign( eLo->dst(), eUp->dst(), eLo->org() ) > 0 ) return false;

		// eUp->Dst is below eLo, so splice eUp->Dst into eLo
		regUp->dirty = regLo->dirty = true;
		e = _mesh->split_edge( eLo );
		if ( e == nullptr )
			throw std::runtime_error( "mesh split failed" );
		_mesh->splice( eUp->lnext(), eLo->sym() );
		e->rface()->set_inside( regUp->inside );
	}
	return true;
}


// Check the upper and lower edges of the given region to see if
// they intersect.  If so, create the intersection and add it
// to the data structures.
//
// Returns true if adding the new intersection resulted in a recursive
// call to add_right_edges(); in this case all "dirty" regions have been
// checked for intersections, and possibly regUp has been deleted.
bool tessellator::check_for_intersect( active_region *regUp )
{
	active_region *regLo = regUp->below();
	half_edge *eUp = regUp->eUp;
	half_edge *eLo = regLo->eUp;
	vertex *orgUp = eUp->org();
	vertex *orgLo = eLo->org();
	vertex *dstUp = eUp->dst();
	vertex *dstLo = eLo->dst();
	double tMinUp, tMaxLo;
	vertex isect, *orgMin;

	assert( ! VertEq( dstLo, dstUp ) );
	assert( EdgeSign( dstUp, _event, orgUp ) <= 0 );
	assert( EdgeSign( dstLo, _event, orgLo ) >= 0 );
	assert( orgUp != _event && orgLo != _event );
	assert( ! regUp->fixUpperEdge && ! regLo->fixUpperEdge );

	if ( orgUp == orgLo )
		return false;	// right endpoints are the same

	tMinUp = std::min( orgUp->y(), dstUp->y() );
	tMaxLo = std::max( orgLo->y(), dstLo->y() );
	if ( tMinUp > tMaxLo )
		return false;	// t ranges do not overlap

	if ( VertLeq( orgUp, orgLo ) )
	{
		if ( EdgeSign( dstLo, orgUp, orgLo ) > 0 ) return false;
	}
	else
	{
		if ( EdgeSign( dstUp, orgLo, orgUp ) < 0 ) return false;
	}

	// At this point the edges intersect, at least marginally

	tesedgeIntersect( dstUp, orgUp, dstLo, orgLo, &isect );
	// The following properties are guaranteed:
	assert( std::min( orgUp->y(), dstUp->y() ) <= isect.y() );
	assert( isect.y() <= std::max( orgLo->y(), dstLo->y() ) );
	assert( std::min( dstLo->x(), dstUp->x() ) <= isect.x() );
	assert( isect.x() <= std::max( orgLo->x(), orgUp->x() ) );

	if ( VertLeq( &isect, _event ) )
	{
		// The intersection point lies slightly to the left of the sweep line,
		// so move it until it''s slightly to the right of the sweep line.
		// (If we had perfect numerical precision, this would never happen
		// in the first place).  The easiest and safest thing to do is
		// replace the intersection by tess->event.
		isect.set( _event->x(), _event->y() );
	}
	// Similarly, if the computed intersection lies to the right of the
	// rightmost origin (which should rarely happen), it can cause
	// unbelievable inefficiency on sufficiently degenerate inputs.
	// (If you have the test program, try running test54.d with the
	// "X zoom" option turned on).
	orgMin = VertLeq( orgUp, orgLo ) ? orgUp : orgLo;
	if ( VertLeq( orgMin, &isect ) )
	{
		isect.set( orgMin->x(), orgMin->y() );
	}

	if ( VertEq( &isect, orgUp ) || VertEq( &isect, orgLo ) )
	{
		// Easy case -- intersection at one of the right endpoints
		( void ) check_for_right_splice( regUp );
		return false;
	}

	if (    ( ! VertEq( dstUp, _event )
	          && EdgeSign( dstUp, _event, &isect ) >= 0 )
	        || ( ! VertEq( dstLo, _event )
	             && EdgeSign( dstLo, _event, &isect ) <= 0 ) )
	{
		// Very unusual -- the new upper or lower edge would pass on the
		// wrong side of the sweep event, or through it.  This can happen
		// due to very small numerical errors in the intersection calculation.
		if ( dstLo == _event )
		{
			// Splice dstLo into eUp, and process the new region(s)
			if ( _mesh->split_edge( eUp->sym() ) == nullptr )
				throw std::runtime_error( "mesh split failed" );
			_mesh->splice( eLo->sym(), eUp );
			regUp = top_left_region( regUp );
			if ( regUp == nullptr )
				throw std::runtime_error( "top left region failed" );
			eUp = regUp->below()->eUp;
			finish_left_regions( regUp->below(), regLo );
			add_right_edges( regUp, eUp->oprev(), eUp, eUp, true );
			return true;
		}
		if ( dstUp == _event )
		{
			// Splice dstUp into eLo, and process the new region(s)
			_mesh->split_edge( eLo->sym() );
			_mesh->splice( eUp->lnext(), eLo->oprev() );
			regLo = regUp;
			regUp = top_right_region( regUp );
			auto e = regUp->below()->eUp->rprev();
			regLo->eUp = eLo->oprev();
			eLo = finish_left_regions( regLo, nullptr );
			add_right_edges( regUp, eLo->onext(), eUp->rprev(), e, true );
			return true;
		}
		// Special case: called from ConnectRightVertex.  If either
		// edge passes on the wrong side of tess->event, split it
		// (and wait for ConnectRightVertex to splice it appropriately).
		if ( EdgeSign( dstUp, _event, &isect ) >= 0 )
		{
			regUp->above()->dirty = regUp->dirty = true;
			_mesh->split_edge( eUp->sym() );
			eUp->org()->set( _event->x(), _event->y() );
		}
		if ( EdgeSign( dstLo, _event, &isect ) <= 0 )
		{
			regUp->dirty = regLo->dirty = true;
			_mesh->split_edge( eLo->sym() );
			eLo->org()->set( _event->x(), _event->y() );
		}
		// leave the rest for ConnectRightVertex
		return false;
	}

	// General case -- split both edges, splice into new vertex.
	// When we do the splice operation, the order of the arguments is
	// arbitrary as far as correctness goes.  However, when the operation
	// creates a new face, the work done is proportional to the size of
	// the new face.  We expect the faces in the processed part of
	// the mesh (ie. eUp->Lface) to be smaller than the faces in the
	// unprocessed original contours (which will be eLo->Oprev->Lface).
	_mesh->split_edge( eUp->sym() );
	_mesh->split_edge( eLo->sym() );
	_mesh->splice( eLo->oprev(), eUp );

	vertex *v = eUp->org();
	v->set( isect.x(), isect.y() );

	_pq.push( v );
	get_intersect_data( v, orgUp, dstUp, orgLo, dstLo );
	regUp->above()->dirty = regUp->dirty = regLo->dirty = true;
	return false;
}

// When the upper or lower edge of any region changes, the region is
// marked "dirty".  This routine walks through all the dirty regions
// and makes sure that the dictionary invariants are satisfied
// (see the comments at the beginning of this file).  Of course
// new dirty regions can be created as we make changes to restore
// the invariants.
void tessellator::walk_dirty_regions( active_region *regUp )
{
	active_region *regLo = regUp->below();

	for ( ;; )
	{
		// Find the lowest dirty region (we walk from the bottom up).
		while ( regLo->dirty )
		{
			regUp = regLo;
			regLo = regLo->below();
		}
		if ( ! regUp->dirty )
		{
			regLo = regUp;
			regUp = regUp->above();
			if ( regUp == nullptr || ! regUp->dirty )
			{
				// We've walked all the dirty regions
				return;
			}
		}
		regUp->dirty = false;
		auto eUp = regUp->eUp;
		auto eLo = regLo->eUp;

		if ( eUp->dst() != eLo->dst() )
		{
			// Check that the edge ordering is obeyed at the Dst vertices.
			if ( check_for_left_splice( regUp ) )
			{

				// If the upper or lower edge was marked fixUpperEdge, then
				// we no longer need it (since these edges are needed only for
				// vertices which otherwise have no right-going edges).
				if ( regLo->fixUpperEdge )
				{
					delete_region( regLo );
					_mesh->delete_edge( eLo );
					regLo = regUp->below();
					eLo = regLo->eUp;
				}
				else if ( regUp->fixUpperEdge )
				{
					delete_region( regUp );
					_mesh->delete_edge( eUp );
					regUp = regLo->above();
					eUp = regUp->eUp;
				}
			}
		}
		if ( eUp->org() != eLo->org() )
		{
			if (    eUp->dst() != eLo->dst()
			        && ! regUp->fixUpperEdge && ! regLo->fixUpperEdge
			        && ( eUp->dst() == _event || eLo->dst() == _event ) )
			{
				// When all else fails in CheckForIntersect(), it uses tess->event
				// as the intersection location.  To make this possible, it requires
				// that tess->event lie between the upper and lower edges, and also
				// that neither of these is marked fixUpperEdge (since in the worst
				// case it might splice one of these edges into tess->event, and
				// violate the invariant that fixable edges are the only right-going
				// edge from their associated vertex).
				if ( check_for_intersect( regUp ) )
				{
					// WalkDirtyRegions() was called recursively; we're done
					return;
				}
			}
			else
			{
				// Even though we can't use CheckForIntersect(), the Org vertices
				// may violate the dictionary edge ordering.  Check and correct this.
				( void ) check_for_right_splice( regUp );
			}
		}
		if ( eUp->org() == eLo->org() && eUp->dst() == eLo->dst() )
		{
			// A degenerate loop consisting of only two edges -- delete it.
			AddWinding( eLo, eUp );
			delete_region( regUp );
			_mesh->delete_edge( eUp );
			regUp = regLo->above();
		}
	}
}


// Purpose: connect a "right" vertex vEvent (one where all edges go left)
// to the unprocessed portion of the mesh.  Since there are no right-going
// edges, two regions (one above vEvent and one below) are being merged
// into one.  "regUp" is the upper of these two regions.
//
// There are two reasons for doing this (adding a right-going edge):
//  - if the two regions being merged are "inside", we must add an edge
//    to keep them separated (the combined region would not be monotone).
//  - in any case, we must leave some record of vEvent in the dictionary,
//    so that we can merge vEvent with features that we have not seen yet.
//    For example, maybe there is a vertical edge which passes just to
//    the right of vEvent; we would like to splice vEvent into this edge.
//
// However, we don't want to connect vEvent to just any vertex.  We don''t
// want the new edge to cross any other edges; otherwise we will create
// intersection vertices even when the input data had no self-intersections.
// (This is a bad thing; if the user's input data has no intersections,
// we don't want to generate any false intersections ourselves.)
//
// Our eventual goal is to connect vEvent to the leftmost unprocessed
// vertex of the combined region (the union of regUp and regLo).
// But because of unseen vertices with all right-going edges, and also
// new vertices which may be created by edge intersections, we don''t
// know where that leftmost unprocessed vertex is.  In the meantime, we
// connect vEvent to the closest vertex of either chain, and mark the region
// as "fixUpperEdge".  This flag says to delete and reconnect this edge
// to the next processed vertex on the boundary of the combined region.
// Quite possibly the vertex we connected to will turn out to be the
// closest one, in which case we won''t need to make any changes.
void tessellator::connect_right_vertex( active_region *regUp, half_edge *eBottomLeft )
{
	half_edge *eNew;
	half_edge *eTopLeft = eBottomLeft->onext();
	active_region *regLo = regUp->below();
	half_edge *eUp = regUp->eUp;
	half_edge *eLo = regLo->eUp;
	int degenerate = false;

	if ( eUp->dst() != eLo->dst() )
		( void ) check_for_intersect( regUp );

	// Possible new degeneracies: upper or lower edge of regUp may pass
	// through vEvent, or may coincide with new intersection vertex
	if ( VertEq( eUp->org(), _event ) )
	{
		_mesh->splice( eTopLeft->oprev(), eUp );
		regUp = top_left_region( regUp );
		if ( regUp == nullptr )
			throw std::runtime_error( "top left region failed" );
		eTopLeft = regUp->below()->eUp;
		finish_left_regions( regUp->below(), regLo );
		degenerate = true;
	}
	if ( VertEq( eLo->org(), _event ) )
	{
		_mesh->splice( eBottomLeft, eLo->oprev() );
		eBottomLeft = finish_left_regions( regLo, nullptr );
		degenerate = true;
	}
	if ( degenerate )
	{
		add_right_edges( regUp, eBottomLeft->onext(), eTopLeft, eTopLeft, true );
		return;
	}

	// Non-degenerate situation -- need to add a temporary, fixable edge.
	// Connect to the closer of eLo->Org, eUp->Org.
	if ( VertLeq( eLo->org(), eUp->org() ) )
		eNew = eLo->oprev();
	else
		eNew = eUp;
	eNew = _mesh->connect( eBottomLeft->lprev(), eNew );
	if ( eNew == nullptr )
		throw std::runtime_error( "mesh connect failed" );

	// Prevent cleanup, otherwise eNew might disappear before we've even
	// had a chance to mark it as a temporary edge.
	add_right_edges( regUp, eNew, eNew->onext(), eNew->onext(), false );
	eNew->sym()->region()->fixUpperEdge = true;
	walk_dirty_regions( regUp );
}

// The event vertex lies exacty on an already-processed edge or vertex.
// Adding the new vertex involves splicing it into the already-processed
// part of the mesh.
void tessellator::connect_left_degenerate( active_region *regUp, vertex *vEvent )
{
	half_edge *e, *eTopLeft, *eTopRight, *eLast;
	active_region *reg;

	e = regUp->eUp;
	if ( VertEq( e->org(), vEvent ) )
	{
		// e->Org is an unprocessed vertex - just combine them, and wait
		// for e->Org to be pulled from the queue
		_mesh->splice( e, vEvent->edge() );
		return;
	}

	if ( ! VertEq( e->dst(), vEvent ) )
	{
		// General case -- splice vEvent into edge e which passes through it
		if ( _mesh->split_edge( e->sym() ) == nullptr )
			throw std::runtime_error( "mesh split failed" );
		if ( regUp->fixUpperEdge )
		{
			// This edge was fixable -- delete unused portion of original edge
			_mesh->delete_edge( e->onext() );
			regUp->fixUpperEdge = false;
		}
		_mesh->splice( vEvent->edge(), e );
		sweep_event( vEvent );	// recurse
		return;
	}

	// vEvent coincides with e->Dst, which has already been processed.
	// Splice in the additional right-going edges.
	regUp = top_right_region( regUp );
	reg = regUp->below();
	eTopRight = reg->eUp->sym();
	eTopLeft = eLast = eTopRight->onext();
	if ( reg->fixUpperEdge )
	{
		// Here e->Dst has only a single fixable edge going right.
		// We can delete it since now we have some real right-going edges.
		assert( eTopLeft != eTopRight );   // there are some left edges too
		delete_region( reg );
		_mesh->delete_edge( eTopRight );
		eTopRight = eTopLeft->oprev();
	}
	_mesh->splice( vEvent->edge(), eTopRight );
	if ( ! EdgeGoesLeft( eTopLeft ) )
	{
		// e->Dst had no left-going edges -- indicate this to AddRightEdges()
		eTopLeft = nullptr;
	}
	add_right_edges( regUp, eTopRight->onext(), eLast, eTopLeft, true );
}


// Purpose: connect a "left" vertex (one where both edges go right)
// to the processed portion of the mesh.  Let R be the active region
// containing vEvent, and let U and L be the upper and lower edge
// chains of R.  There are two possibilities:
//
// - the normal case: split R into two regions, by connecting vEvent to
//   the rightmost vertex of U or L lying to the left of the sweep line
//
// - the degenerate case: if vEvent is close enough to U or L, we
//   merge vEvent into that edge chain.  The subcases are:
//	- merging with the rightmost vertex of U or L
//	- merging with the active edge of U or L
//	- merging with an already-processed portion of U or L
void tessellator::connect_left_vertex( vertex *vEvent )
{
	active_region *regUp, *regLo, *reg;
	active_region tmp;

	// assert( vEvent->edge()->Onext->Onext == vEvent->edge() );

	// Get a pointer to the active region containing vEvent
	tmp.eUp = vEvent->edge()->sym();
	regUp = _dict->search( &tmp )->key();
	regLo = regUp->below();
	if ( !regLo )
	{
		// This may happen if the input polygon is coplanar.
		return;
	}
	half_edge *eUp = regUp->eUp;
	half_edge *eLo = regLo->eUp;

	// Try merging with U or L first
	if ( std::equal_to<double>()(EdgeSign( eUp->dst(), vEvent, eUp->org() ), 0) )
	{
		connect_left_degenerate( regUp, vEvent );
		return;
	}

	// Connect vEvent to rightmost processed vertex of either chain.
	// e->Dst is the vertex that we will connect to vEvent.
	reg = VertLeq( eLo->dst(), eUp->dst() ) ? regUp : regLo;

	if ( regUp->inside || reg->fixUpperEdge )
	{
		half_edge *eNew;
		if ( reg == regUp )
		{
			eNew = _mesh->connect( vEvent->edge()->sym(), eUp->lnext() );
			if ( eNew == nullptr )
				throw std::runtime_error( "mesh connect failed" );
		}
		else
		{
			half_edge *tempHalfEdge = _mesh->connect( eLo->dnext(), vEvent->edge() );
			if ( tempHalfEdge == nullptr )
				throw std::runtime_error( "mesh connect failed" );

			eNew = tempHalfEdge->sym();
		}

		if ( reg->fixUpperEdge )
		{
			if ( !fix_upper_edge( reg, eNew ) )
				throw std::runtime_error( "fix upper edge failed" );
		}
		else
			compute_winding( add_region_below( regUp, eNew ) );
		sweep_event( vEvent );
	}
	else
	{
		// The new vertex is in a region which does not belong to the polygon.
		// We don''t need to connect this vertex to the rest of the mesh.
		add_right_edges( regUp, vEvent->edge(), vEvent->edge(), nullptr, true );
	}
}


// Does everything necessary when the sweep line crosses a vertex.
// Updates the mesh and the edge dictionary.
void tessellator::sweep_event( vertex *vEvent )
{
	active_region *regUp, *reg;
	half_edge *e, *eTopLeft, *eBottomLeft;

	_event = vEvent; // for access in EdgeLeq()

	// Check if this vertex is the right endpoint of an edge that is
	// already in the dictionary.  In this case we don't need to waste
	// time searching for the location to insert new edges.
	e = vEvent->edge();
	while ( e->region() == nullptr )
	{
		e = e->onext();
		if ( e == vEvent->edge() )
		{
			// All edges go right -- not incident to any processed edges
			connect_left_vertex( vEvent );
			return;
		}
	}

	// Processing consists of two phases: first we "finish" all the
	// active regions where both the upper and lower edges terminate
	// at vEvent (ie. vEvent is closing off these regions).
	// We mark these faces "inside" or "outside" the polygon according
	// to their winding number, and delete the edges from the dictionary.
	// This takes care of all the left-going edges from vEvent.
	regUp = top_left_region( e->region() );
	if ( regUp == nullptr )
		throw std::runtime_error( "top left region failed" );
	reg = regUp->below();
	eTopLeft = reg->eUp;
	eBottomLeft = finish_left_regions( reg, nullptr );

	// Next we process all the right-going edges from vEvent.  This
	// involves adding the edges to the dictionary, and creating the
	// associated "active regions" which record information about the
	// regions between adjacent dictionary edges.
	if ( eBottomLeft->onext() == eTopLeft )
	{
		// No right-going edges -- add a temporary "fixable" edge
		connect_right_vertex( regUp, eBottomLeft );
	}
	else
		add_right_edges( regUp, eBottomLeft->onext(), eTopLeft, eTopLeft, true );
}


// Make the sentinel coordinates big enough that they will never be
// merged with real input features.

// We add two sentinel edges above and below all other edges,
// to avoid special cases at the top and bottom.
void tessellator::add_sentinel( double xmin, double xmax, double y )
{
	half_edge *e = _mesh->make_edge();
	e->org()->set( xmax, y );
	e->dst()->set( xmin, y );

	_event = e->dst();

	active_region *reg = _regionPool.allocate();
	reg->eUp = e;
	reg->windingNumber = 0;
	reg->inside = false;
	reg->fixUpperEdge = false;
	reg->sentinel = true;
	reg->dirty = false;
	reg->nodeUp = _dict->insert( reg );
	if ( reg->nodeUp == nullptr )
		throw std::runtime_error( "dictionary insert failed" );
}


// We maintain an ordering of edge intersections with the sweep line.
// This order is maintained in a dynamic dictionary.
void tessellator::init_edge_dict( void )
{
	_dict = new dict<active_region*>( [=](active_region *r1, active_region *r2) -> bool { return edge_leq( r1, r2 ); } );

	double w = ( _bmax[0] - _bmin[0] );
	double h = ( _bmax[1] - _bmin[1] );

	double smin = _bmin[0] - w;
	double smax = _bmax[0] + w;
	double tmin = _bmin[1] - h;
	double tmax = _bmax[1] + h;

	add_sentinel( smin, smax, tmin );
	add_sentinel( smin, smax, tmax );
}


void tessellator::done_edge_dict( void )
{
#ifndef NDEBUG
	int fixedEdges = 0;
#endif
	active_region *reg;
	while ( ( reg = _dict->min()->key() ) != nullptr )
	{
		// At the end of all processing, the dictionary should contain
		// only the two sentinel edges, plus at most one "fixable" edge
		// created by ConnectRightVertex().
		if ( ! reg->sentinel )
		{
			assert( reg->fixUpperEdge );
			assert( ++fixedEdges == 1 );
		}
		assert( reg->windingNumber == 0 );
		delete_region( reg );
	}
	delete _dict;
	_dict = nullptr;
}

// Insert all vertices into the priority queue which determines the
// order in which vertices cross the sweep line.
bool tessellator::init_priorityq( void )
{
	vertex *vHead = _mesh->vertex_list();
	size_t count = 0;
	for ( vertex *v = vHead->next(); v != vHead; v = v->next() )
		count++;
	// Make sure there is enough space for sentinels.
	count += 8; // MAX( 8, tess->alloc.extraVertices );

	_pq.reserve( count );

	vHead = _mesh->vertex_list();
	vertex *v;
	for ( v = vHead->next(); v != vHead; v = v->next() )
		_pq.push( v );

	_pq.init();
	if ( v != vHead )
		return false;

	return true;
}


void tessellator::done_priorityq( void )
{
	_pq.clear();
}


// tessComputeInterior( tess ) computes the planar arrangement specified
// by the given contours, and further subdivides this arrangement
// into regions.  Each region is marked "inside" if it belongs
// to the polygon, according to the rule given by tess->windingRule.
// Each interior region is guaranteed be monotone.
bool tessellator::compute_interior( void )
{
	vertex *v, *vNext;

	// Each vertex defines an event for our sweep line.  Start by inserting
	// all the vertices in a priority queue.  Events are processed in
	// lexicographic order, ie.
	//	e1 < e2  iff  e1.x < e2.x || (e1.x == e2.x && e1.y < e2.y)
	_mesh->remove_degenerate_edges();
	if ( !init_priorityq() )
		return false; // if error
	init_edge_dict();

	v = _pq.top();
	while ( v != nullptr )
	{
		_pq.pop();
		while ( true )
		{
			vNext = _pq.top();
			if ( vNext == nullptr || ! VertEq( vNext, v ) )
				break;

			// Merge together all vertices at exactly the same location.
			// This is more efficient than processing them one at a time,
			// simplifies the code (see ConnectLeftDegenerate), and is also
			// important for correct handling of certain degenerate cases.
			// For example, suppose there are two identical edges A and B
			// that belong to different contours (so without this code they would
			// be processed by separate sweep events).  Suppose another edge C
			// crosses A and B from above.  When A is processed, we split it
			// at its intersection point with C.  However this also splits C,
			// so when we insert B we may compute a slightly different
			// intersection point.  This might leave two edges with a small
			// gap between them.  This kind of error is especially obvious
			// when using boundary extraction (TESS_BOUNDARY_ONLY).
			_pq.pop();
			_mesh->splice( v->edge(), vNext->edge() );
		}
		sweep_event( v );
		v = _pq.top();
	}

	// Set tess->event for debugging purposes
	_event = _dict->min()->key()->eUp->org();
	done_edge_dict();
	done_priorityq();

	if ( !_mesh->remove_degenerate_faces() )
		return false;
	_mesh->check_mesh();

	return true;
}

// We've computed a new intersection point, now we need a "data" pointer
// from the user so that we can refer to this new vertex in the
// rendering callbacks.
void tessellator::get_intersect_data( vertex *isect,
		vertex *orgUp, vertex *dstUp,
		vertex *orgLo, vertex *dstLo )
{
	double weights[4];

	isect->set( 0, 0 );
	VertexWeights( isect, orgUp, dstUp, &weights[0] );
	VertexWeights( isect, orgLo, dstLo, &weights[2] );
}

// Invariants for the Edge Dictionary.
// - each pair of adjacent edges e2=Succ(e1) satisfies EdgeLeq(e1,e2)
//   at any valid location of the sweep event
// - if EdgeLeq(e2,e1) as well (at any valid sweep event), then e1 and e2
//   share a common endpoint
// - for each e, e->Dst has been processed, but not e->Org
// - each edge e satisfies VertLeq(e->Dst,event) && VertLeq(event,e->Org)
//   where "event" is the current sweep line event.
// - no edge e has zero length
//
// Invariants for the Mesh (the processed portion).
// - the portion of the mesh left of the sweep line is a planar graph,
//   ie. there is *some* way to embed it in the plane
// - no processed edge has zero length
// - no two processed vertices have identical coordinates
// - each "inside" region is monotone, ie. can be broken into two chains
//   of monotonically increasing vertices according to VertLeq(v1,v2)
//   - a non-invariant: these chains may intersect (very slightly)
//
// Invariants for the Sweep.
// - if none of the edges incident to the event vertex have an activeRegion
//   (ie. none of these edges are in the edge dictionary), then the vertex
//   has only right-going edges.
// - if an edge is marked "fixUpperEdge" (it is a temporary edge introduced
//   by ConnectRightVertex), then it is the only right-going edge from
//   its associated vertex.  (This says that these edges exist only
//   when it is necessary.)

// When we merge two edges into one, we need to compute the combined
// winding of the new edge.

}
