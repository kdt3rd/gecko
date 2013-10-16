
#pragma once

#include <string>
#include <vector>
#include <memory>

#include "screen.h"
#include "window.h"
#include "dispatcher.h"

namespace platform
{

////////////////////////////////////////

class system
{
public:
	system( std::string name, std::string desc );
	virtual ~system( void );

	const std::string &name( void ) const { return _name; }
	const std::string &description( void ) const { return _desc; }

	virtual std::vector<std::shared_ptr<screen>> screens( void ) = 0;
	virtual std::shared_ptr<window> new_window( void ) = 0;

	virtual std::shared_ptr<dispatcher> dispatch( void ) = 0;

private:
	std::string _name;
	std::string _desc;
};

////////////////////////////////////////

}
