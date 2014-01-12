
#pragma once

#include <core/point.h>
#include "polyline.h"

namespace draw
{

////////////////////////////////////////

void add_quadratic( const core::point &p1, const core::point &p2, const core::point &p3, polyline &line );
void add_cubic( const core::point &p1, const core::point &p2, const core::point &p3, const core::point &p4, polyline &line );

////////////////////////////////////////

}

