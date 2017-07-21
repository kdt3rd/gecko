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

/// @brief Layout which vertically aligns two areas.
class label_layout : public layout
{
public:
	label_layout( const std::shared_ptr<area> &l, const std::shared_ptr<area> &e );

	/// @brief Compute the minimum size of this layout.
	void compute_bounds( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;
private:
	std::weak_ptr<area> _label;
	std::weak_ptr<area> _entry;
};

////////////////////////////////////////

}

