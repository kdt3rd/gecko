
#pragma once

#include <type_traits>
#include <draw/canvas.h>
#include <layout/simple_area.h>
#include <platform/keyboard.h>
#include "context.h"

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

	~widget( void )
	{
	}

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas )
	{
	}

	virtual bool mouse_press( const base::point &p, int button )
	{
		return false;
	}

	virtual bool mouse_release( const base::point &p, int button )
	{
		return false;
	}

	virtual bool mouse_move( const base::point &p )
	{
		return false;
	}

	virtual bool key_press( platform::scancode c )
	{
		return false;
	}

	virtual bool key_release( platform::scancode c )
	{
		return false;
	}

	virtual bool text_input( char32_t c )
	{
		return false;
	}

	void invalidate( const base::rect &r )
	{
		context::current().invalidate( r );
	}

	void invalidate( void )
	{
		context::current().invalidate( *this );
	}

protected:
	template<typename D>
	void callback_helper( const std::function<void(void)> &cb, D &d )
	{
		d.callback( cb );
	}

	template<typename D, typename ...Args>
	void callback_helper( const std::function<void(void)> &cb, D &d, Args &...args )
	{
		d.callback( cb );
		callback_helper( cb, args... );
	}

	template<typename ...Args>
	void callback_invalidate( Args &...args )
	{
		auto cb = [this]() { invalidate(); };
		callback_helper( cb, args... );
	}
};

////////////////////////////////////////

}

