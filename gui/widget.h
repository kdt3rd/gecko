
#pragma once

#include <type_traits>
#include <draw/canvas.h>
#include <layout/simple_area.h>
#include "delegate.h"

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

	virtual void set_delegate( delegate *d )
	{
		_delegate = d;
	}

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas )
	{
	}

	virtual bool mouse_press( const draw::point &p, int button )
	{
		return false;
	}

	virtual bool mouse_release( const draw::point &p, int button )
	{
		return false;
	}

	virtual bool mouse_move( const draw::point &p )
	{
		return false;
	}

	void invalidate( const draw::rect &r )
	{
		if ( _delegate )
			_delegate->invalidate( r );
	}

	void invalidate( void )
	{
		if ( _delegate )
			_delegate->invalidate( *this );
	}

protected:
	delegate *_delegate = nullptr;
};

////////////////////////////////////////

}

