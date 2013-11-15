
#pragma once

#include <type_traits>
#include <reaction/reaction.h>
#include <draw/canvas.h>

namespace gui
{

////////////////////////////////////////

class widget : public layout::simple_area
{
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

	virtual bool mouse_press( const draw::point &p, int button )
	{
		if ( _action )
			return _action->mouse_press( *this, p, button );
		return false;
	}

	virtual bool mouse_release( const draw::point &p, int button )
	{
		if ( _action )
			return _action->mouse_release( *this, p, button );
		return false;
	}

	virtual bool mouse_move( const draw::point &p )
	{
		if ( _action )
			return _action->mouse_move( *this, p );
		return false;
	}

protected:
	std::unique_ptr<reaction::reaction> _action;
};

////////////////////////////////////////

}

