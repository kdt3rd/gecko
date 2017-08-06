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

class field;

////////////////////////////////////////

class form : public layout
{
public:
	form( const std::shared_ptr<area> &sublayout );

	void add( const std::shared_ptr<field> &field );

	/// @brief Compute the minimum size of this layout.
	void compute_bounds( void ) override;

	/// @brief Compute the position and size of children
	void compute_layout( void ) override;

private:
	std::shared_ptr<area> _layout;
	std::list<std::shared_ptr<field>> _fields;
	double _min_field_w = 0.0;
};

////////////////////////////////////////

}

