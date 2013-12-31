
#pragma once

#include <type_traits>
#include <draw/canvas.h>
#include <layout/simple_area.h>
#include "delegate.h"

namespace model
{
	template<typename T> class datum;
}

namespace gui
{

////////////////////////////////////////

class widget : public layout::simple_area
{
public:
	template<typename T> using datum = model::datum<T>;
	template<typename T> using shared_datum = model::datum<std::shared_ptr<T>>;

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
	template<typename D>
	void callback_helper( const std::function<void(void)> &cb, D &d )
	{
		d += cb;
	}

	template<typename D, typename ...Args>
	void callback_helper( const std::function<void(void)> &cb, D &d, Args &...args )
	{
		d += cb;
		callback_helper( cb, args... );
	}

	template<typename ...Args>
	void callback_invalidate( Args &...args )
	{
		auto cb = [this]() { invalidate(); };
		callback_helper( cb, args... );
	}

	delegate *_delegate = nullptr;
};

////////////////////////////////////////

}

