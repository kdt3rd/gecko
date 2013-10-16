
#pragma once

#include <platform/system.h>

namespace sdl
{

////////////////////////////////////////

class system : public platform::system
{
public:
	system( void );
	virtual ~system( void );

	virtual std::vector<std::shared_ptr<platform::screen>> screens( void ) { return _screens; }
	virtual std::shared_ptr<platform::window> new_window( void );

	virtual std::shared_ptr<platform::dispatcher> dispatch( void );

private:
	std::vector<std::shared_ptr<platform::screen>> _screens;
};

////////////////////////////////////////

}

