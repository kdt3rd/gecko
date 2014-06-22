
#pragma once

#include <base/point.h>
#include "polyline.h"

namespace draw
{

////////////////////////////////////////

size_t circle_precision( double r );

void add_quadratic( const base::point &p1, const base::point &p2, const base::point &p3, polyline &line );
void add_cubic( const base::point &p1, const base::point &p2, const base::point &p3, const base::point &p4, polyline &line );
void add_arc( const base::point &center, double radius, double a1, double a2, polyline &line );

////////////////////////////////////////

}

