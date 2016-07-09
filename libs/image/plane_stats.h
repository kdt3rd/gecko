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
#include "accum_buf.h"
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

// TODO: combine these into one local operator (mean/variance/skewness/kurtosis)?
plane local_mean( const plane &p, int radius );
plane local_variance( const plane &p, int radius );

/// scales the plane coming in as the power provided, so 1 gives the
/// SAT of just p, 2 gives the square, 3 and 4 can give skewness and
/// kurtosis
accum_buf sum_area_table( const plane &p, int power );

/// Summed Area Table optimized form of local mean
plane local_mean( const accum_buf &sat, int radius );
/// Summed Area Table optimized form of local variance
plane local_variance( const accum_buf &sat, const accum_buf &sat2, int radius );

plane mse( const plane &p1, const plane &p2, int radius );

/// Computes a histogram of the plane
///
/// pre-normalizes based on the low/high range and then computes bins
engine::computed_value<std::vector<uint64_t>> histogram( const plane &p, int bins, float lowVal, float highVal );

void add_plane_stats( engine::registry &r );

} // namespace image



