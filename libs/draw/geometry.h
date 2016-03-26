
#pragma once

#include "polyline.h"

namespace draw
{

////////////////////////////////////////

size_t circle_precision( float r );

void add_quadratic( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3, polyline &line );
void add_cubic( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3, const gl::vec2 &p4, polyline &line );
void add_arc( const gl::vec2 &center, float radius, float a1, float a2, polyline &line );

////////////////////////////////////////

}

