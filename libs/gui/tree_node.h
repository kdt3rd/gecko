//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "container.h"

namespace gui
{

////////////////////////////////////////

class tree_node : public container<tree_layout>
{
public:
	using container<tree_layout>::container;

	bool collapsed( void ) const { return _collapsed; }
	void set_collapsed( bool c = true );

	void compute_minimum( void ) override;
	void compute_layout( void ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_release( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

private:
	bool _collapsed = false;
};

////////////////////////////////////////

}
