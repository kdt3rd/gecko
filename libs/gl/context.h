
#pragma once

#include <exception>
#include <memory>
#include <vector>
#include <core/contract.h>
#include "opengl.h"
#include "enums.h"
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "matrix4.h"
#include "vertex_array.h"
#include <core/color.h>

namespace gl
{

////////////////////////////////////////

class context
{
public:
	context( void );
	~context( void );

	void enable( capability cap );
	void disable( capability cap );

	void clear_color( const core::color &c );

	void clear( buffer_bit bit );
	void clear( buffer_bits bits = ( buffer_bit::COLOR_BUFFER_BIT | buffer_bit::DEPTH_BUFFER_BIT ) );

	void depth_mask( bool write );
	void stencil_mask( bool write );
	void stencil_mask( uint32_t mask );
	void color_mask( bool r, bool g, bool b, bool a );
	void blend_func( blend_style src, blend_style dst );

	void save_matrix( void );
	void ortho( float left, float right, float top, float bottom );
	void scale( float x, float y, float z = 1.F );
	void translate( float dx, float dy, float dz = 0.F );
	void multiply( const matrix4 &m );

	matrix4 current_matrix( void )
	{
		precondition( !_matrix.empty(), "no matrix available");
		return _matrix.back();
	}

	void restore_matrix( void );


	template<typename ...Args>
	std::shared_ptr<shader> new_shader( Args &&...args )
	{
		return std::make_shared<shader>( std::forward<Args>( args )... );
	}

	template<typename ...Args>
	std::shared_ptr<program> new_program( Args &&...args )
	{
		return std::make_shared<program>( std::forward<Args>( args )... );
	}

	void use_program( const std::shared_ptr<program> &p = std::shared_ptr<program>() )
	{
		if ( p )
			glUseProgram( p->_program );
		else
			glUseProgram( 0 );
	}

	template<typename ...Args>
	std::shared_ptr<texture> new_texture( Args &&...args )
	{
		return std::make_shared<texture>( std::forward<Args>( args )... );
	}

	template<typename D, typename ...Args>
	std::shared_ptr<buffer<D>> new_buffer( Args &&...args )
	{
		return std::make_shared<buffer<D>>( std::forward<Args>( args )... );
	}

	template<typename ...Args>
	std::shared_ptr<vertex_array> new_vertex_array( Args &&...args )
	{
		return std::make_shared<vertex_array>( std::forward<Args>( args )... );
	}

	void enable_vertex_attrib_array( program::attribute attr, size_t size, data_type dt, size_t stride = 0, const void *data = nullptr )
	{
		glEnableVertexAttribArray( attr );
		glVertexAttribPointer( attr, size, static_cast<GLenum>( dt ), GL_FALSE, stride, data );
	}

	void disable_vertex_attrib_array( program::attribute attr )
	{
		glDisableVertexAttribArray( attr );
	}

private:
	std::vector<matrix4> _matrix;
};

////////////////////////////////////////

}

