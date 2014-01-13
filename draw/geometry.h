
#pragma once

#include <core/point.h>
#include "polyline.h"

namespace draw
{

////////////////////////////////////////

size_t circle_precision( double r );

void add_quadratic( const core::point &p1, const core::point &p2, const core::point &p3, polyline &line );
void add_cubic( const core::point &p1, const core::point &p2, const core::point &p3, const core::point &p4, polyline &line );
void add_arc( const core::point &center, double radius, double a1, double a2, polyline &line );

////////////////////////////////////////

}

