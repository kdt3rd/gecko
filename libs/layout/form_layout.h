//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "layout.h"

namespace layout
{

////////////////////////////////////////

class form_layout : public layout
{
public:
	form_layout( const std::shared_ptr<area> &labels );

	std::shared_ptr<area> add( const std::shared_ptr<area> &entry, const std::shared_ptr<area> &label );

	/// @brief Compute the minimum size of this layout.
	void compute_bounds( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;

private:
	std::shared_ptr<area> _labels;
	std::list<std::weak_ptr<area>> _areas;
	double _min_entry_w = 0.0;
};

////////////////////////////////////////

}

