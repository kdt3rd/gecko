//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "window.h"
#include "container.h"

namespace gui
{

////////////////////////////////////////

class menu : public window
{
public:
	menu( const std::shared_ptr<platform::window> &w );
	virtual ~menu( void );

	void add_entry( const std::string &n );

private:
	std::shared_ptr<gui::simple_container> _container;
};

////////////////////////////////////////

}

