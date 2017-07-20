//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "layout.h"
#include <base/alignment.h>
#include <list>
#include <memory>

namespace layout
{

////////////////////////////////////////

class box_layout : public layout
{
public:
	box_layout( base::alignment direction = base::alignment::RIGHT );

	void add( const std::shared_ptr<area> &a ) { _areas.push_back( a ); }

	/// @brief Compute the minimum size of this layout.
	void compute_bounds( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;

private:
	std::list<std::weak_ptr<area>> _areas;
	base::alignment _align = base::alignment::RIGHT;
};

////////////////////////////////////////

}

