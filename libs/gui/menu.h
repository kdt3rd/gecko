
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

	void add_entry( const std::string &n );

private:
	std::shared_ptr<gui::simple_container> _container;
};

////////////////////////////////////////

}

