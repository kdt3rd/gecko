
#pragma once

#include <string>
#include <memory>
#include <functional>

#include "painter.h"

namespace platform
{

////////////////////////////////////////

class window
{
public:
	window( void );
	virtual ~window( void );

	virtual void raise( void ) = 0;
//	virtual void lower( void ) = 0;

	virtual void show( void ) = 0;
	virtual void hide( void ) = 0;
	virtual bool is_visible( void ) = 0;

//	virtual rect geometry( void ) = 0;
//	virtual void set_geometry( const rect &r ) = 0;
	virtual void resize( double w, double h ) = 0;
	virtual void set_minimum_size( double w, double h ) = 0;

	virtual void set_title( const std::string &t ) = 0;
//	virtual void set_icon( const icon &i );

	virtual std::shared_ptr<painter> paint( void ) = 0;

	void when_closed( std::function<void(void)> f ) { _closed = f; }
	void when_shown( std::function<void(void)> f ) { _shown = f; }
	void when_hidden( std::function<void(void)> f ) { _hidden = f; }
	void when_minimized( std::function<void(void)> f ) { _minimized = f; }
	void when_maximized( std::function<void(void)> f ) { _maximized = f; }
	void when_restored( std::function<void(void)> f ) { _restored = f; }
	void when_exposed( std::function<void()> f ) { _exposed = f; }
	void when_moved( std::function<void(double,double)> f ) { _moved = f; }
	void when_resized( std::function<void(double,double)> f ) { _resized = f; }

	void closed( void ) { if ( _closed ) _closed(); }
	void shown( void ) { if ( _shown ) _shown(); }
	void hidden( void ) { if ( _hidden ) _hidden(); }
	void minimized( void ) { if ( _minimized ) _minimized(); }
	void maximized( void ) { if ( _maximized ) _maximized(); }
	void restored( void ) { if ( _restored ) _restored(); }
	void exposed( void ) { if ( _exposed ) _exposed(); }
	void moved( double x, double y ) { if ( _moved ) _moved( x, y ); }
	void resized( double w, double h ) { if ( _resized ) _resized( w, h ); }

private:
	std::function<void(void)> _closed;
	std::function<void(void)> _shown;
	std::function<void(void)> _hidden;
	std::function<void(void)> _minimized;
	std::function<void(void)> _maximized;
	std::function<void(void)> _restored;
	std::function<void(void)> _exposed;
	std::function<void(double,double)> _moved;
	std::function<void(double,double)> _resized;
};

////////////////////////////////////////

}

