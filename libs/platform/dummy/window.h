
#pragma once

#include <platform/window.h>

namespace platform { namespace dummy
{

////////////////////////////////////////

class window : public platform::window
{
public:
	window( void );
	~window( void );

	void raise( void ) override;
	void lower( void ) override;

	void show( void ) override;
	void hide( void ) override;
	bool is_visible( void ) override;

//	rect geometry( void ) override;
//	void set_position( double x, double y ) override;
	void resize( double w, double h ) override;
	void set_minimum_size( double w, double h ) override;

	void set_title( const std::string &t ) override;
//	void set_icon( const icon &i ) override;

	void invalidate( const core::rect &r ) override;

	gl::context context( void ) override { return gl::context(); }
	std::shared_ptr<gldraw::canvas> canvas( void ) override;

	double width( void ) override
	{
		return 0.0;
	}

	double height( void ) override
	{
		return 0.0;
	}

private:
	void update_canvas( double w, double h );
};

////////////////////////////////////////

} }

