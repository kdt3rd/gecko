//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "layout.h"
#include <memory>

namespace layout
{

////////////////////////////////////////

class border_layout : public layout
{
public:
	border_layout( void );

	void set_top( const std::shared_ptr<area> &a ) { _top = a; }
	void set_bottom( const std::shared_ptr<area> &a ) { _bottom = a; }
	void set_left( const std::shared_ptr<area> &a ) { _left = a; }
	void set_right( const std::shared_ptr<area> &a ) { _right = a; }
	void set_center( const std::shared_ptr<area> &a ) { _center = a; }

	/// @brief Compute the minimum size of this layout.
	void compute_minimum( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;

private:
	std::weak_ptr<area> _top;
	std::weak_ptr<area> _bottom;
	std::weak_ptr<area> _left;
	std::weak_ptr<area> _right;
	std::weak_ptr<area> _center;
};

////////////////////////////////////////

}

