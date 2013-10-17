
#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"

namespace xcb
{

////////////////////////////////////////

class dispatcher : public platform::dispatcher
{
public:
	dispatcher( void );
	virtual ~dispatcher( void );

	int execute( void );
	void exit( int code );

	void add_window( const std::shared_ptr<window> &w );

private:
	int _exit_code = 0;
	std::map<uint32_t, std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

}
