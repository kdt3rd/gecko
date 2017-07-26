/// @cond LIBTESS
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
// Converted to C++ and greatly simplified (for a specific use case) by Ian Godin - 2016

#pragma once

#include <base/memory_pool.h>
#include <base/priority_queue.h>

namespace draw
{

////////////////////////////////////////

template<typename Key> class dict;
class tess_mesh;
class active_region;
class half_edge;
class vertex;

class tessellator
{
public:
	tessellator( const std::function<void(double,double)> &add_point, const std::function<void(size_t,size_t,size_t)> &add_tri );
	~tessellator( void );

	void *begin_contour( void );
	void contour_point( void *&contour, double x, double y );
	void end_contour( void *&contour );

	void tessellate( void );

private:
	bool is_winding_inside( int n )
	{
		return ( n & 1 );
	}

	void compute_bounding_box( void );
	void output_polymesh( void );

	bool edge_leq( active_region *reg1, active_region *reg2 );

	void delete_region( active_region *reg );
	int fix_upper_edge( active_region *reg, half_edge *newEdge );

	active_region *top_left_region( active_region *reg );
	active_region *top_right_region( active_region *reg );
	active_region *add_region_below( active_region *regAbove, half_edge *eNewUp );
	void compute_winding( active_region *reg );
	void finish_region( active_region *reg );
	half_edge *finish_left_regions( active_region *regFirst, active_region *regLast );
	void add_right_edges( active_region *regUp, half_edge *eFirst, half_edge *eLast, half_edge *eTopLeft, bool cleanUp );
	bool check_for_right_splice( active_region *regUp );
	bool check_for_left_splice( active_region *regUp );
	bool check_for_intersect( active_region *regUp );
	void walk_dirty_regions( active_region *regUp );
	void connect_right_vertex( active_region *regUp, half_edge *eBottomLeft );
	void connect_left_vertex( vertex *vEvent );
	void connect_left_degenerate( active_region *regUp, vertex *vEvent );
	void sweep_event( vertex *vEvent );
	void add_sentinel( double smin, double smax, double t );
	void init_edge_dict( void );
	void done_edge_dict( void );
	bool init_priorityq( void );
	void done_priorityq( void );

	// tessComputeInterior( tess ) computes the planar arrangement specified
	// by the given contours, and further subdivides this arrangement
	// into regions.  Each region is marked "inside" if it belongs
	// to the polygon, according to the rule given by tess->windingRule.
	// Each interior region is guaranteed be monotone.
	bool compute_interior( void );

	void get_intersect_data( vertex *isect, vertex *orgUp, vertex *dstUp, vertex *orgLo, vertex *dstLo );

	// state needed for collecting the input data

	// stores the input contours, and eventually the tessellation itself
	tess_mesh	*_mesh;

	// state needed for projecting onto the sweep plane

	double _bmin[2];
	double _bmax[2];

	// state needed for the line sweep

	// edge dictionary for sweep line
	dict<active_region*> *_dict;

	// priority queue of vertex events
	base::priority_queue<vertex *> _pq;

	// current sweep event being processed
	vertex *_event;

	base::memory_pool<active_region,256> _regionPool;

	std::function<void(double,double)> _add_point;
	std::function<void(size_t,size_t,size_t)> _add_tri;
};

////////////////////////////////////////

}
/// @endcond
