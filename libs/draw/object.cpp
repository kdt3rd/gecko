
#include "object.h"
#include "mesh.h"
#include "polylines.h"

namespace draw
{

////////////////////////////////////////

object::object( void )
{
}

////////////////////////////////////////

void object::create( const std::shared_ptr<canvas> &c, const base::path &path, const base::paint &paint )
{
	polylines lines;
	path.replay( lines );

	if ( paint.get_stroke_width() != 0.0 )
	{
		_stroke_prog = c->program( "color_mesh.vert", "single_color.frag" );
		_stroke_prog->set_uniform( "color", paint.get_stroke_color() );
		_stroke = c->new_vertex_array();
		auto mesh = lines.stroked( paint.get_stroke_width() ).filled();
		mesh.set_attrib_pointers( *c, _stroke, _stroke_prog->get_attribute_location( "position" ) );
		_stroke_parts = mesh.parts();
	}

	if ( paint.has_fill_color() )
	{
		_fill_prog = c->program( "color_mesh.vert", "single_color.frag" );
		_fill_prog->set_uniform( "color", paint.get_fill_color() );
		_fill = c->new_vertex_array();
		auto mesh = lines.filled();
		mesh.set_attrib_pointers( *c, _fill, _fill_prog->get_attribute_location( "position" ) );
		_fill_parts = mesh.parts();
	}
	else if ( paint.has_fill_linear() )
	{
		_fill_prog = c->program( "position_uv.vert", "linear_gradient.frag" );
		_fill_prog->set_uniform( "txt", 0 );
		_fill_prog->set_uniform( "origin", paint.get_fill_linear_origin() );
		_fill_prog->set_uniform( "dir", paint.get_fill_linear_size() );
		_fill_texture = c->gradient( paint.get_fill_linear_gradient() );
		_fill = c->new_vertex_array();
		auto mesh = lines.filled();
		mesh.set_attrib_pointers( *c, _fill, _fill_prog->get_attribute_location( "position" ) );
		_fill_parts = mesh.parts();
	}
	else if ( paint.has_fill_radial() )
	{
		_fill_prog = c->program( "position_uv.vert", "radial_gradient.frag" );
		_fill_prog->set_uniform( "txt", 0 );
		_fill_prog->set_uniform( "center", paint.get_fill_radial_p1() );
		_fill_prog->set_uniform( "radius", paint.get_fill_radial_r2() );
		_fill_texture = c->gradient( paint.get_fill_radial_gradient() );
		_fill = c->new_vertex_array();
		auto mesh = lines.filled();
		mesh.set_attrib_pointers( *c, _fill, _fill_prog->get_attribute_location( "position" ) );
		_fill_parts = mesh.parts();
	}
	else if ( paint.has_fill_conical() )
	{
		_fill_prog = c->program( "position_uv.vert", "conical_gradient.frag" );
		_fill_prog->set_uniform( "txt", 0 );
		_fill_prog->set_uniform( "center", paint.get_fill_conical_center() );
		_fill_texture = c->gradient( paint.get_fill_conical_gradient() );
		_fill_texture->bind( gl::texture::target::TEXTURE_RECTANGLE ).set_wrapping( gl::wrapping::REPEAT );
		_fill = c->new_vertex_array();
		auto mesh = lines.filled();
		mesh.set_attrib_pointers( *c, _fill, _fill_prog->get_attribute_location( "position" ) );
		_fill_parts = mesh.parts();
	}
	else if ( paint.has_no_fill() )
	{
	}
	else
		throw std::runtime_error( "unhandled fill type" );
}

////////////////////////////////////////

void object::draw( gl::api &ogl )
{
	using target = gl::texture::target;

	// Draw fill
	if ( _fill )
	{
		gl::bound_texture tx;
		if ( _fill_texture )
			tx = _fill_texture->bind( target::TEXTURE_RECTANGLE, 0 );

		if ( _fill_prog )
		{
			ogl.use_program( _fill_prog );
			_fill_prog->set_uniform( "mvp_matrix", ogl.current_matrix() );
		}

		{
			auto va = _fill->bind();
			for ( auto part: _fill_parts )
				va.draw( std::get<0>( part ), std::get<1>( part ), std::get<2>( part ) );
		}
	}

	// Draw stroke
	if ( _stroke )
	{
		if ( _stroke_prog )
		{
			ogl.use_program( _stroke_prog );
			_stroke_prog->set_uniform( "mvp_matrix", ogl.current_matrix() );
		}

		{
			auto va = _stroke->bind();
			for ( auto part: _stroke_parts )
				va.draw( std::get<0>( part ), std::get<1>( part ), std::get<2>( part ) );
		}
	}

	ogl.use_program();
}

////////////////////////////////////////

}

