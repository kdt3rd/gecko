
#pragma once

#include <exception>
#include <memory>
#include <vector>
#include "opengl.h"
#include "enums.h"
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_array.h"
#include <draw/color.h>

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

	void clear_color( const draw::color &c );

	void clear( buffer_bit bit );
	void clear( buffer_bits bits = ( buffer_bit::COLOR_BUFFER_BIT | buffer_bit::DEPTH_BUFFER_BIT ) );

	void depth_mask( bool write );
	void stencil_mask( bool write );
	void stencil_mask( uint32_t mask );
	void color_mask( bool r, bool g, bool b, bool a );

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
		std::make_shared<texture>( std::forward<Args>( args )... );
	}

	template<texture::target T>
	bound_texture<T> bind_texture( const std::shared_ptr<texture> &tx )
	{
		return bound_texture<T>( tx->_texture );
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
};

////////////////////////////////////////

}

