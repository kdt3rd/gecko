// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "float_ops.h"

#include "registry.h"

////////////////////////////////////////

namespace engine
{
////////////////////////////////////////

void register_float_ops( registry &r )
{
    r.add( op(
        "d.castto",
        []( float v ) -> double { return static_cast<double>( v ); },
        op::simple ) );
    r.add( op(
        "d.castfrom",
        []( double v ) -> float { return static_cast<float>( v ); },
        op::simple ) );

    r.add( op(
        "f.negate", []( float v ) -> float { return -v; }, op::simple ) );
    r.add( op(
        "d.negate", []( double v ) -> double { return -v; }, op::simple ) );

    r.add( op(
        "f.add",
        []( float a, float b ) -> float { return a + b; },
        op::simple ) );
    r.add( op(
        "d.add",
        []( double a, double b ) -> double { return a + b; },
        op::simple ) );

    r.add( op(
        "f.sub",
        []( float a, float b ) -> float { return a - b; },
        op::simple ) );
    r.add( op(
        "d.sub",
        []( double a, double b ) -> double { return a - b; },
        op::simple ) );

    r.add( op(
        "f.mul",
        []( float a, float b ) -> float { return a * b; },
        op::simple ) );
    r.add( op(
        "d.mul",
        []( double a, double b ) -> double { return a * b; },
        op::simple ) );

    r.add( op(
        "f.div",
        []( float a, float b ) -> float { return a / b; },
        op::simple ) );
    r.add( op(
        "d.div",
        []( double a, double b ) -> double { return a / b; },
        op::simple ) );
}

////////////////////////////////////////

} // namespace engine
