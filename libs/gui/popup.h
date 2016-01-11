
#pragma once

#include "window.h"

namespace gui
{

////////////////////////////////////////

class popup : public window
{
public:
	popup( const std::shared_ptr<platform::window> &w );
	virtual ~popup( void );
};

////////////////////////////////////////

}

