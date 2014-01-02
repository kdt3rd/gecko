
#pragma once

#include <platform/window.h>
#include <draw/cairo/canvas.h>

namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of window.
class window : public platform::window
{
public:
	window( void );
	virtual ~window( void );

	virtual void raise( void );
	virtual void lower( void );

	virtual void show( void );
	virtual void hide( void );
	virtual bool is_visible( void );

//	virtual rect geometry( void );
//	virtual void set_position( double x, double y );
	virtual void resize( double w, double h );
	virtual void set_minimum_size( double w, double h );

	virtual void set_title( const std::string &t );
//	virtual void set_icon( const icon &i );

	virtual std::shared_ptr<draw::canvas> canvas( void );

	virtual void resized( double w, double h );
	virtual void exposed( void );

	virtual void invalidate( const draw::rect &r );

	void set_ns( void *nswin, void *nsview );

	virtual double width( void ) { return _last_w; }
	virtual double height( void ) { return _last_h; }

private:
	struct objcwrapper;
	objcwrapper *_impl;

	std::shared_ptr<cairo::canvas> _canvas;
	double _last_w = 0.0, _last_h = 0.0;
};

////////////////////////////////////////

}

