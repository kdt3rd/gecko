// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace scene
{
/// TBD: rman has solid geometry ops (difference, intersection, union)
enum class primitive_type
{
    transform,      // meta node used for transform all of it's children at once
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
    group,   ///< can be used to group primitives together
    instance ///< instance of another named / id primitive, but has it's own additional values
};

} // namespace scene
