
#pragma once

#include "point.h"
#include "polyline.h"

namespace gldraw
{

////////////////////////////////////////

void add_quadratic( const point &p1, const point &p2, const point &p3, polyline &line );
void add_cubic( const point &p1, const point &p2, const point &p3, const point &p4, polyline &line );

////////////////////////////////////////

}

