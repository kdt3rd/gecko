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

/// @brief Layout which packs areas arounds the sides of a shrinking center area.
///
/// This layout puts it's areas along the sides in the order added.
/// The widget will fill the selectd side, but will be at minimum size in the orthogonal direction.
/// The last area can be put in the center, taking the rest of the unused space.
class packing_layout : public layout
{
public:
	/// @brief Default constructor.
	packing_layout( void );

	/// @brief Add area to one side.
	//
	/// @param a Area to add.
	/// @param where Which side to add to.
	void add( const std::shared_ptr<area> &a, base::alignment where );

	void compute_bounds( void ) override;

	void compute_layout( void ) override;

private:
	struct section
	{
		section( const std::shared_ptr<area> &ar, base::alignment al )
			: _area( ar ), _align( al )
		{
		}

		std::weak_ptr<area> _area;
		base::alignment _align;
	};

	std::list<section> _areas;
};

////////////////////////////////////////

}
