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
class field_layout : public layout
{
public:
	field_layout( const std::shared_ptr<area> &l, const std::shared_ptr<area> &e );

	double field_minimum_width( void );

	void set_field_width( double w )
	{
		_width = w;
	}

	/// @brief Compute the minimum size of this layout.
	void compute_bounds( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;

private:
	double _width = 0.0;
	std::weak_ptr<area> _label;
	std::weak_ptr<area> _field;
};

////////////////////////////////////////

}

