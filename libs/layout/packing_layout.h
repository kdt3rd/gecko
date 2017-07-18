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

class packing_layout : public layout
{
public:
	packing_layout( void );

	void add( const std::shared_ptr<area> &a, base::alignment where ) { _areas.emplace_back( a, where ); }

	/// @brief Compute the minimum size of this layout.
	void compute_minimum( void ) override;

	/// @brief Compute the position and size of children
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

