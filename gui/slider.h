
#pragma once

#include "widget.h"
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class slider : public widget
{
public:
	slider( void );
	~slider( void );

	void set_pressed( bool p );

	virtual void paint( const std::shared_ptr<draw::canvas> &c );

	virtual void layout( void );

private:
	bool _pressed = false;
};

////////////////////////////////////////

}

