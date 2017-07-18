//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "layout.h"
#include <list>
#include <memory>

namespace layout
{

////////////////////////////////////////

class hbox_layout : public layout
{
public:
	hbox_layout( void );

	void add( const std::shared_ptr<area> &a ) { _areas.push_back( a ); }

	/// @brief Compute the minimum size of this layout.
	void compute_minimum( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;

private:
	std::list<std::weak_ptr<area>> _areas;
};

////////////////////////////////////////

}

