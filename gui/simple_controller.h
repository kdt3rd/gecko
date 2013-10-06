
#pragma once

#include "controller.h"
#include "area.h"
#include <memory>

////////////////////////////////////////

class simple_controller : public controller
{
public:
	bool process_event( event &e, bool focused );

	void set_focus( const std::shared_ptr<controller> &f ) { _focus = f; }
	void add_location( const std::shared_ptr<area> &a, const std::shared_ptr<controller> &c ) { _locations.emplace_back( a, c ); }

private:
	std::shared_ptr<controller> _focus;
	std::vector<std::pair<std::shared_ptr<area>,std::shared_ptr<controller>>> _locations;
};

////////////////////////////////////////

