//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <base/contract.h>
#include "plane.h"
#include "op_registry.h"

////////////////////////////////////////

namespace image
{

inline engine::computed_value<double> sum( const plane &p )
{
	engine::dimensions d;
	d.x = 1;
	return engine::computed_value<double>( op_registry(), "p.sum", d, p );
}

inline engine::computed_value<double> sum( plane &&p )
{
	engine::dimensions d;
	d.x = 1;
	return engine::computed_value<double>( op_registry(), "p.sum", d, std::move( p ) );
}

// TODO: combine these
plane local_mean( const plane &p, int radius );
plane local_variance( const plane &p, int radius );

plane mse( const plane &p1, const plane &p2, int radius );

void add_plane_stats( engine::registry &r );

} // namespace image



