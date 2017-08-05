//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "layout.h"
#include <vector>

namespace layout
{

////////////////////////////////////////

class scroll_layout : public layout
{
public:
	void compute_bounds( void ) override;

	void compute_layout( void ) override;

	void set_main( const std::shared_ptr<area> &a )
	{
		_main = a;
	}

	void set_hscroll( const std::shared_ptr<area> &a )
	{
		_hscroll = a;
	}

	void set_vscroll( const std::shared_ptr<area> &a )
	{
		_vscroll = a;
	}

	void set_corner( const std::shared_ptr<area> &a )
	{
		_corner = a;
	}

private:
	std::weak_ptr<area> _main;
	std::weak_ptr<area> _hscroll;
	std::weak_ptr<area> _vscroll;
	std::weak_ptr<area> _corner;
};

////////////////////////////////////////

}

