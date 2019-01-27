// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace scene
{

class time
{
public:
	int64_t offset( void ) const;
	double sub_frame( void ) const;
	const sample_rate &rate( void ) const;
};

} // namespace scene

