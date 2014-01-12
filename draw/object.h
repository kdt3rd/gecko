
#pragma once

#include "drawable.h"
#include "mesh.h"

namespace draw
{

////////////////////////////////////////

class object : public drawable
{
public:
	object( void );

	template<typename ...Args, typename ...Names>
	void set_mesh( gl::context &ctxt, const mesh<Args...> &mesh, Names ...namelist )
	{
		static_assert( sizeof...(Names) == sizeof...(Args), "invalid number of program attributes" );
		precondition( !_vao, "already attached to vertex array" );
		_vao = ctxt.new_vertex_array();
		mesh.set_attrib_pointers( ctxt, _vao, namelist... );
		_parts = mesh.parts();
	}

	void set_program( const std::shared_ptr<gl::program> &p )
	{
		_program = p;
	}

	void draw( gl::context &ctxt ) override;

private:
	std::shared_ptr<gl::program> _program;
	std::shared_ptr<gl::vertex_array> _vao;
	std::vector<std::tuple<gl::primitive,size_t,size_t>> _parts;
};

////////////////////////////////////////

}

