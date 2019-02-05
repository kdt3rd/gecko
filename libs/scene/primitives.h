// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace scene
{

enum class primitive
{
    convex_polygon, // special case for performance? rman has this
    general_polygon, // concave or with holes
    bilinear_patch,
    bicubic_patch, // requires a basis for u / v (bezier, bspline, catmullrom, hermite, power)
    nurb_patch,
    subdiv_surface,
    quadric_cone,
    quadric_cylinder,
    quadric_disk,
    quadric_hyperboloid,
    quadric_paraboloid,
    quadric_sphere,
    quadric_torus,
    point,
    curve,
    blobby,
    volume,
    procedural,
    group, ///< can be used to group primitives together
    instance ///< instance of another named / id primitive, but has it's own additional values
};

/// TBD: rman has solid geometry ops (difference, intersection, union)
///
/// @brief Class primitives provides...
///
class primitives
{
public:
	primitives( void );
	virtual ~primitives( void );
private:

};

} // namespace scene



