
#include "color_wheel.h"
#include "geometry.h"

namespace
{
	constexpr double PI = 3.14159265358979323846;
}

namespace draw
{

////////////////////////////////////////

color_wheel::color_wheel( void )
{
}

////////////////////////////////////////

void color_wheel::create( const std::shared_ptr<canvas> &c, const base::point &center, double radius )
{
	std::vector<float> coords = { float(center.x()), float(center.y()), 0.75F, 0.75F, 0.75F };
	_data_size = circle_precision( radius );
	base::color color;
	for ( size_t i = 0; i <= _data_size; ++i )
	{
		double m = double(i) / double(_data_size);
		double a = m * PI * 2.0;
		auto p = center + base::point::polar( radius, a );
		coords.push_back( static_cast<float>( p.x() ) );
		coords.push_back( static_cast<float>( p.y() ) );

		color.set_hsl( a, 1.0, 0.4 );
		coords.push_back( static_cast<float>( color.red() ) );
		coords.push_back( static_cast<float>( color.green() ) );
		coords.push_back( static_cast<float>( color.blue() ) );
	}

	_fill_prog = c->program( "color_mesh.vert", "color_mesh.frag" );
	_data = c->new_vertex_array();

	auto buf = c->new_array_buffer<float>( coords );
	auto va = _data->bind();
	va.attrib_pointer( _fill_prog->get_attribute_location( "position" ), buf, 2, 5, 0 );
	va.attrib_pointer( _fill_prog->get_attribute_location( "color" ), buf, 3, 5, 2 );
	_data_size += 2;
}

////////////////////////////////////////

void color_wheel::draw( gl::api &ogl )
{
	ogl.use_program( _fill_prog );
	_fill_prog->set_uniform( "mvp_matrix", ogl.current_matrix() );

	{
		auto va = _data->bind();
		va.draw( gl::primitive::TRIANGLE_FAN, 0, _data_size );
	}

	ogl.use_program();
}

////////////////////////////////////////

}

