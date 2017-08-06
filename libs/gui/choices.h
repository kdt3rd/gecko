//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "composite.h"
#include <layout/grid_layout.h>

namespace gui
{

class label;
class radio_button;

////////////////////////////////////////

class choices : public composite
{
public:
	choices( void );

	void add_choice( const std::string &label );

protected:
	void for_subwidgets( const std::function<void(const std::shared_ptr<widget>&)> &f ) override;

	void choose( size_t c );

private:
	std::shared_ptr<layout::grid_layout> _layout;
	std::vector<std::shared_ptr<label>> _labels;
	std::vector<std::shared_ptr<radio_button>> _radios;
};

////////////////////////////////////////

}

