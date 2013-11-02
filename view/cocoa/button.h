
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

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas );

private:
	std::shared_ptr<draw::area> _area;
};

////////////////////////////////////////

}
