
#pragma once

#include <type_traits>
#include <reaction/reaction.h>
#include <draw/canvas.h>

namespace gui
{

////////////////////////////////////////

template<typename action>
class widget : public action
{
	static_assert( std::is_base_of<reaction::reaction, action>::value, "action class must be derived from reaction::reaction" );

public:
	widget( void )
	{
	}

	virtual ~widget( void )
	{
	}

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas )
	{
	}

private:
};

////////////////////////////////////////

}

