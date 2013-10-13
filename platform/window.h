
#pragma once

#include <string>
#include <memory>
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

	virtual void set_title( const std::string &t ) = 0;
//	virtual void set_icon( const icon &i );

	virtual std::shared_ptr<painter> paint( void ) = 0;
};

////////////////////////////////////////

}

