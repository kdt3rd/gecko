Tessellator
===========

Brief overview of the algorithm used in the tessellator.


Phases of the algorithm
-----------------------

1. Using a line-sweep algorithm, partition the plane into x-monotone
   regions.  Any vertical line intersects an x-monotone region in
   at most one interval.
2. Triangulate the x-monotone regions.
3. Group the triangles into strips and fans.

The Line Sweep
--------------

There are three data structures: the mesh, the event queue, and the
edge dictionary.

The mesh is a "quad-edge" data structure which records the topology of
the current decomposition; for details see the include file "mesh.h".

The event queue simply holds all vertices (both original and computed
ones), organized so that we can quickly extract the vertex with the
minimum x-coord (and among those, the one with the minimum y-coord).

The edge dictionary describes the current intersection of the sweep
line with the regions of the polygon.  This is just an ordering of the
edges which intersect the sweep line, sorted by their current order of
intersection.  For each pair of edges, we store some information about
the monotone region between them -- these are call "active regions"
(since they are crossed by the current sweep line).

The basic algorithm is to sweep from left to right, processing each
vertex.  The processed portion of the mesh (left of the sweep line) is
a planar decomposition.  As we cross each vertex, we update the mesh
and the edge dictionary, then we check any newly adjacent pairs of
edges to see if they intersect.

A vertex can have any number of edges.  Vertices with many edges can
be created as vertices are merged and intersection points are
computed.  For unprocessed vertices (right of the sweep line), these
edges are in no particular order around the vertex; for processed
vertices, the topological ordering should match the geometric ordering.

The vertex processing happens in two phases: first we process are the
left-going edges (all these edges are currently in the edge
dictionary).  This involves:

 - deleting the left-going edges from the dictionary;
 - relinking the mesh if necessary, so that the order of these edges around
   the event vertex matches the order in the dictionary;
 - marking any terminated regions (regions which lie between two left-going
   edges) as either "inside" or "outside" according to their winding number.

When there are no left-going edges, and the event vertex is in an
"interior" region, we need to add an edge (to split the region into
monotone pieces).  To do this we simply join the event vertex to the
rightmost left endpoint of the upper or lower edge of the containing
region.

Then we process the right-going edges.  This involves:

 - inserting the edges in the edge dictionary;
 - computing the winding number of any newly created active regions.
   We can compute this incrementally using the winding of each edge
   that we cross as we walk through the dictionary.
 - relinking the mesh if necessary, so that the order of these edges around
   the event vertex matches the order in the dictionary;
 - checking any newly adjacent edges for intersection and/or merging.

If there are no right-going edges, again we need to add one to split
the containing region into monotone pieces.  In our case it is most
convenient to add an edge to the leftmost right endpoint of either
containing edge; however we may need to change this later (see the
code for details).


Invariants
----------

These are the most important invariants maintained during the sweep.
We define a function VertLeq(v1,v2) which defines the order in which
vertices cross the sweep line, and a function EdgeLeq(e1,e2; loc)
which says whether e1 is below e2 at the sweep event location "loc".
This function is defined only at sweep event locations which lie
between the rightmost left endpoint of {e1,e2}, and the leftmost right
endpoint of {e1,e2}.

Invariants for the Edge Dictionary.

 - Each pair of adjacent edges e2=Succ(e1) satisfies EdgeLeq(e1,e2)
   at any valid location of the sweep event.
 - If EdgeLeq(e2,e1) as well (at any valid sweep event), then e1 and e2
   share a common endpoint.
 - For each e in the dictionary, e->Dst has been processed but not e->Org.
 - Each edge e satisfies VertLeq(e->Dst,event) && VertLeq(event,e->Org)
   where "event" is the current sweep line event.
 - No edge e has zero length.
 - No two edges have identical left and right endpoints.

Invariants for the Mesh (the processed portion).

 - The portion of the mesh left of the sweep line is a planar graph,
   ie. there is *some* way to embed it in the plane.
 - No processed edge has zero length.
 - No two processed vertices have identical coordinates.
 - Each "inside" region is monotone, ie. can be broken into two chains
   of monotonically increasing vertices according to VertLeq(v1,v2)
   - a non-invariant: these chains may intersect (slightly) due to
     numerical errors, but this does not affect the algorithm's operation.

Invariants for the Sweep.

 - If a vertex has any left-going edges, then these must be in the edge
   dictionary at the time the vertex is processed.
 - If an edge is marked "fixUpperEdge" (it is a temporary edge introduced
   by ConnectRightVertex), then it is the only right-going edge from
   its associated vertex.  (This says that these edges exist only
   when it is necessary.)


Robustness
----------

The key to the robustness of the algorithm is maintaining the
invariants above, especially the correct ordering of the edge
dictionary.  We achieve this by:

  1. Writing the numerical computations for maximum precision rather
     than maximum speed.

  2. Making no assumptions at all about the results of the edge
     intersection calculations -- for sufficiently degenerate inputs,
     the computed location is not much better than a random number.

  3. When numerical errors violate the invariants, restore them
     by making *topological* changes when necessary (ie. relinking
     the mesh structure).


Triangulation and Grouping
--------------------------

We finish the line sweep before doing any triangulation.  This is
because even after a monotone region is complete, there can be further
changes to its vertex data because of further vertex merging.

After triangulating all monotone regions, we want to group the
triangles into fans and strips.  We do this using a greedy approach.
The triangulation itself is not optimized to reduce the number of
primitives; we just try to get a reasonable decomposition of the
computed triangulation.
