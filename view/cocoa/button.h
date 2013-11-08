
#pragma once

#include <view/view.h>

namespace cocoa
{

////////////////////////////////////////

class button : public view::view
{
public:
	button( const std::shared_ptr<draw::area> &a );
	virtual ~button( void );

	virtual void layout( const std::shared_ptr<draw::canvas> &canvas );
	virtual void paint( const std::shared_ptr<draw::canvas> &canvas );

	void set_pressed( bool p );

private:
	bool _pressed = false;
	std::shared_ptr<draw::area> _area;
};

////////////////////////////////////////

}
