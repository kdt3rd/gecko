//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "spectral_density.h"
#include <vector>
#include "triple.h"

////////////////////////////////////////

namespace color
{


struct luminous_intensity_function
{
	using value_type = double;
	using table = std::vector<value_type>;
	
	std::vector<value_type> x_bar;
	std::vector<value_type> y_bar;
	std::vector<value_type> z_bar;

	triple<value_type> integrate

namespace two_deg
{

const luminous_intensity_function &cie_1931_cmf( void );
const luminous_intensity_function &cie_1931_judd_1951( void );
const luminous_intensity_function &cie_1931_judd_vos( void );

/// LMS cone fundamental intensity functions (not yet ratified?)
/// x == l, y == m, z == s
const luminous_intensity_function &cie_2012_xyz_cmf( void );

}

namespace ten_deg
{

const luminous_intensity_function &cie_1964_cmf( void );

/// LMS cone fundamental intensity function (not yet ratified)
/// as for two deg
const luminous_intensity_function &cie_2012_xyz_cmf( void );

}

} // namespace color



