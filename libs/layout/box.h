//
// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include "layout.h"

#include <list>
#include <memory>

namespace layout
{

////////////////////////////////////////

/// @brief Layout that strings areas in a line.
///
/// This layout puts all areas side by side in a particular direction.
class box : public layout
{
public:
	/// @brief Default constructor.
	box( alignment direction = alignment::RIGHT );

	/// @brief Add an area.
	/// @param a Area to add.
	void add( const std::shared_ptr<area> &a ) { _areas.push_back( a ); }

	void compute_bounds( void ) override;

	void compute_layout( void ) override;

private:
	std::list<std::weak_ptr<area>> _areas;
	alignment _align = alignment::RIGHT;
};

////////////////////////////////////////

}

