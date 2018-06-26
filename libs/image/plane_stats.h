//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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
	d.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(double) );
	return engine::computed_value<double>( op_registry(), "p.sum", d, p );
}

inline engine::computed_value<double> sum( plane &&p )
{
	engine::dimensions d;
	d.bytes_per_item = static_cast<engine::dimensions::value_type>( sizeof(double) );
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
/// Computes structured similarity
/// if sigma is negative, it auto computes it based on the radius
plane ssim( const plane &p1, const plane &p2, int radius, float L = 1.f, float k1 = 0.01f, float k2 = 0.03f, float sigma = -1.f );

/// Computes a histogram of the plane
///
/// pre-normalizes based on the low/high range and then computes bins
engine::computed_value<std::vector<uint64_t>> histogram( const plane &p, int bins, float lowVal, float highVal );

void add_plane_stats( engine::registry &r );

} // namespace image



