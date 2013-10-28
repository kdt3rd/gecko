
#pragma once

#include <view/view.h>

namespace cocoa
{

////////////////////////////////////////

class button : public view::view
{
public:
	button( void );
	virtual ~button( void );

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas, const draw::rect &area );
};

////////////////////////////////////////

}
