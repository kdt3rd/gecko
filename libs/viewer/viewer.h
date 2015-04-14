
#pragma once

#include <gui/widget.h>

namespace viewer
{

////////////////////////////////////////

class viewer : public gui::widget
{
public:
	viewer( void );

	void set_texture_a( const std::shared_ptr<gl::texture> &t );
	void set_texture_b( const std::shared_ptr<gl::texture> &t );

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	void compute_minimum( void ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;
	bool mouse_release( const base::point &p, int button ) override;
	bool mouse_wheel( int amount ) override;

private:
	std::shared_ptr<gl::texture> _textureA;
	std::shared_ptr<gl::texture> _textureB;
	std::shared_ptr<gl::program> _prog;
	std::shared_ptr<gl::vertex_array> _quad;

	base::point _panA;
	base::point _panB;
	int _zoomA = 1;
	int _zoomB = 1;

	bool _panningA = false;
	bool _panningB = false;
	base::point _last;
};


////////////////////////////////////////

}

