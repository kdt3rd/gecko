
#include "mesh.h"

namespace gl
{

////////////////////////////////////////

mesh::binding::binding( binding &&other )
	: _elements( other._elements ), _bound( std::move( other._bound ) ), _mesh( other._mesh ), _vao( other._vao ), _prog( other._prog )
{
	other._prog = nullptr;
	other._vao = nullptr;
	other._mesh = nullptr;
}

////////////////////////////////////////

mesh::binding::~binding( void )
{
	_prog = nullptr;
	_vao = nullptr;
}

////////////////////////////////////////

void mesh::binding::bind_elements( element_buffer_data &data )
{
	_bound.bind_elements( data.ebo() );
}

////////////////////////////////////////

void mesh::binding::draw( void )
{
	if ( _elements )
	{
		for ( const auto &t: _mesh->_prims )
			_bound.draw_elements( std::get<0>( t ), std::get<1>( t ), std::get<2>( t ) );
	}
	else
	{
		for ( const auto &t: _mesh->_prims )
			_bound.draw_arrays( std::get<0>( t ), std::get<1>( t ), std::get<2>( t ) );
	}
}

////////////////////////////////////////

mesh::binding::binding( mesh *m, vertex_array *vao, program *prog )
	: _bound( vao->bind() )
{
	_elements = vao->has_element_buffer();
	_mesh = m;
	_vao = vao;
	_prog = prog;
	_prog->use();
}

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

gl::program &mesh::get_program( void )
{
	if ( !_prog )
	{
		api ogl;
		_prog = ogl.new_program();
	}
	return *_prog;
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

mesh::binding mesh::bind( void )
{
	precondition( _prog, "program not created" );
	return binding( this, _vao.get(), _prog.get() );
}

////////////////////////////////////////

}

