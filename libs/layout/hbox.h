//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "layout.h"

namespace layout
{

////////////////////////////////////////

class hbox : public layout
{
public:
	hbox( void );

	void add( area &a, double fraction = 0.0 );

	void update( void ) override;
	void reset( void ) override;

private:
	void setup( void );

	bool _init = false;
	double _default_spacing = 3.0;
	variable _spacing;
	double _default_padding = 3.0;
	variable _padding;

	expression _current;
};

////////////////////////////////////////

}

