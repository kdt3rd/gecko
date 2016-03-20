
#include "mesh.h"

namespace gl
{

////////////////////////////////////////

mesh::mesh( void )
{
	api ogl;
	_vao = ogl.new_vertex_array();
}

////////////////////////////////////////

mesh::~mesh( void )
{
}

////////////////////////////////////////

void mesh::add_triangles( size_t count, size_t start )
{
	_prims.emplace_back( primitive::TRIANGLES, start, count );
}

////////////////////////////////////////

void mesh::add_triangle_strip( size_t count, size_t start )
{
	_prims.emplace_back( primitive::TRIANGLE_STRIP, start, count );
}

////////////////////////////////////////

void mesh::add_triangle_fan( size_t count, size_t start )
{
	_prims.emplace_back( primitive::TRIANGLE_FAN, start, count );
}

////////////////////////////////////////

void mesh::add_lines( size_t count, size_t start )
{
	_prims.emplace_back( primitive::LINES, start, count );
}

////////////////////////////////////////

void mesh::add_line_strip( size_t count, size_t start )
{
	_prims.emplace_back( primitive::LINE_STRIP, start, count );
}

////////////////////////////////////////

void mesh::add_line_loop( size_t count, size_t start )
{
	_prims.emplace_back( primitive::LINE_LOOP, start, count );
}

////////////////////////////////////////

void mesh::add_elements( primitive p, index_buffer_data &data )
{
	_vao->bind().bind_elements( data.ibo() );
	switch ( p )
	{
		case primitive::POINTS:
			break;
		case primitive::TRIANGLES:
			add_triangles( data.size() );
			break;
		case primitive::TRIANGLE_FAN:
			add_triangle_fan( data.size() );
			break;
		case primitive::TRIANGLE_STRIP:
			add_triangle_strip( data.size() );
			break;
		case primitive::LINES:
			add_lines( data.size() );
			break;
		case primitive::LINE_LOOP:
			add_line_loop( data.size() );
			break;
		case primitive::LINE_STRIP:
			add_line_strip( data.size() );
			break;
	}
}

////////////////////////////////////////

void mesh::draw( void )
{
	precondition( _prog, "program not created" );
	precondition( _vao, "vertex array not created" );
	_prog->use();

	auto bb = _vao->bind();
	if ( _vao->has_index_buffer() )
	{
		for ( const auto &t: _prims )
			bb.draw_elements( std::get<0>( t ), std::get<1>( t ), std::get<2>( t ) );
	}
	else
	{
		for ( const auto &t: _prims )
			bb.draw_arrays( std::get<0>( t ), std::get<1>( t ), std::get<2>( t ) );
	}
}

////////////////////////////////////////

}

