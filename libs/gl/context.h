
#pragma once

#include "opengl.h"
#include "enums.h"
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "matrix4.h"
#include "vertex_array.h"

#include <base/contract.h>
#include <base/color.h>

#include <exception>
#include <memory>
#include <vector>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL context
class context
{
public:
	/// @brief Constructor
	context( void );

	/// @brief Destructor
	~context( void );

	/// @brief Get OpenGL vendor
	std::string get_vendor( void );

	/// @brief Get OpenGL renderer
	std::string get_renderer( void );

	/// @brief Get OpenGL version
	std::string get_version( void );

	/// @brief Get OpenGL shading version
	std::string get_shading_version( void );

	size_t get_max_uniform_buffer_bindings( void );

	/// @brief Enable server-side GL capabilities
	void enable( capability cap );

	/// @brief Disable server-side GL capabilities
	void disable( capability cap );

	/// @brief Set the clear color
	void clear_color( const base::color &c );

	/// @brief Clear a buffer
	void clear( buffer_bit bit );

	/// @brief Clear buffers
	void clear( buffer_bits bits = ( buffer_bit::COLOR_BUFFER_BIT | buffer_bit::DEPTH_BUFFER_BIT ) );

	/// @brief Enable depth mask writing
	void depth_mask( bool write );

	/// @brief Enable stencil mask writing
	void stencil_mask( bool write );

	/// @brief Enable stencil mask writing
	void stencil_mask( uint32_t mask );

	/// @brief Set a color mask
	void color_mask( bool r, bool g, bool b, bool a );

	/// @brief Set the blending function
	void blend_func( blend_style src, blend_style dst );

	/// @brief Depth function to use
	void depth_func( depth_test t );

	/// @brief Set the viewport coordinates.
	void viewport( int64_t x, int64_t y, size_t w, size_t h );

	/// @brief Save the current matrix
	void save_matrix( void );

	/// @brief Set an orthographic projection
	void ortho( float left, float right, float top, float bottom );

	/// @brief Scale matrix
	void scale( float x, float y, float z = 1.F );

	/// @brief Translate matrix
	void translate( float dx, float dy, float dz = 0.F );

	/// @brief Multiply matrix
	void multiply( const matrix4 &m );

	/// @brief Get the current matrix
	matrix4 current_matrix( void )
	{
		precondition( !_matrix.empty(), "no matrix available" );;;
		return _matrix.back();
	}

	/// @brief Restore a saved matrix
	void restore_matrix( void );


	/// @brief Construct a new shader
	template<typename ...Args>
	std::shared_ptr<shader> new_shader( Args &&...args )
	{
		return std::make_shared<shader>( std::forward<Args>( args )... );
	}

	/// @brief Construct a new program
	template<typename ...Args>
	std::shared_ptr<program> new_program( Args &&...args )
	{
		return std::make_shared<program>( std::forward<Args>( args )... );
	}

	/// @brief Use a program
	void use_program( const std::shared_ptr<program> &p = std::shared_ptr<program>() )
	{
		if ( p )
			glUseProgram( p->_program );
		else
			glUseProgram( 0 );
	}

	/// @brief Construct a new texture
	template<typename ...Args>
	std::shared_ptr<texture> new_texture( Args &&...args )
	{
		return std::make_shared<texture>( std::forward<Args>( args )... );
	}

	/// @brief Construct a new array buffer
	template<typename D, typename ...Args>
	std::shared_ptr<buffer<D>> new_array_buffer( void )
	{
		return std::make_shared<buffer<D>>( buffer_target::ARRAY_BUFFER );
	}

	/// @brief Construct a new array buffer
	template<typename D, typename ...Args>
	std::shared_ptr<buffer<D>> new_array_buffer( const D *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		return std::make_shared<buffer<D>>( buffer_target::ARRAY_BUFFER, data, n, u );
	}

	/// @brief Construct a new array buffer
	template<typename D, typename ...Args>
	std::shared_ptr<buffer<D>> new_array_buffer( const std::vector<D> &data, buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		return std::make_shared<buffer<D>>( buffer_target::ARRAY_BUFFER, data, u );
	}

	/// @brief Construct a new element buffer
	template<typename D, typename ...Args>
	std::shared_ptr<buffer<D>> new_element_buffer( void )
	{
		return std::make_shared<buffer<D>>( buffer_target::ELEMENT_ARRAY_BUFFER );
	}

	/// @brief Construct a new vertex array
	template<typename ...Args>
	std::shared_ptr<vertex_array> new_vertex_array( Args &&...args )
	{
		return std::make_shared<vertex_array>( std::forward<Args>( args )... );
	}

	/*
	/// @brief Enable vertex array
	void enable_vertex_attrib_array( program::attribute attr, size_t size, data_type dt, size_t stride = 0, const void *data = nullptr )
	{
		glEnableVertexAttribArray( attr );
		glVertexAttribPointer( attr, static_cast<GLint>(size), static_cast<GLenum>( dt ), GL_FALSE, static_cast<GLsizei>(stride), data );
	}

	/// @brief Disable vertex array
	void disable_vertex_attrib_array( program::attribute attr )
	{
		glDisableVertexAttribArray( attr );
	}
	*/

private:
	std::vector<matrix4> _matrix;
};

////////////////////////////////////////

}

