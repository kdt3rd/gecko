//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "polyline.h"

namespace draw
{

////////////////////////////////////////

size_t circle_precision( float r );

void add_quadratic( const polyline::point &p1, const polyline::point &p2, const polyline::point &p3, polyline &line );
void add_cubic( const polyline::point &p1, const polyline::point &p2, const polyline::point &p3, const polyline::point &p4, polyline &line );
void add_arc( const polyline::point &center, float radius, float a1, float a2, polyline &line );

////////////////////////////////////////

}

