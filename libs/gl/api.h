//
// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "opengl.h"
#include "enums.h"
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "matrix4.h"
#include "vertex_buffer.h"
#include "vertex_array.h"
#include "color.h"

#include <base/contract.h>
#include <base/rect.h>

#include <exception>
#include <memory>
#include <vector>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL functions
class api
{
public:
	/// @brief Constructor
	api( void );

	/// @brief Destructor
	~api( void );

	/// @brief Get OpenGL vendor
	std::string get_vendor( void );

	/// @brief Get OpenGL renderer
	std::string get_renderer( void );

	/// @brief Get OpenGL version
	std::string get_version( void );

	/// @brief Get OpenGL shading version
	std::string get_shading_version( void );

	/// @brief Get the number of UBOs allowed
	size_t get_max_uniform_buffer_bindings( void );

	/// @brief Enable server-side GL capabilities
	void enable( capability cap );

	/// @brief Disable server-side GL capabilities
	void disable( capability cap );

	/// @brief Set the clear color
	void clear_color( const color &c );

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
	void viewport( float x, float y, float w, float h );

	/// @brief Set the viewport coordinates.
	void push_scissor( int x, int y, int w, int h );

	void pop_scissor( void );

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

	/// @brief Get the current matrix.
	const matrix4 &model_matrix( void ) const
	{
		precondition( !_matrix.empty(), "no matrix available" );
		return _matrix.back();
	}

	/// @brief Get the current matrix.
	matrix4 &model_matrix( void )
	{
		precondition( !_matrix.empty(), "no matrix available" );
		return _matrix.back();
	}

	void set_model( const matrix4 &m )
	{
		precondition( !_matrix.empty(), "no matrix available" );
		_matrix.back() = m;
	}

	const matrix4 &view_matrix( void ) const
	{
		return _view;
	}

	void set_view( const matrix4 &m )
	{
		_view = m;
	}

	const matrix4 &projection_matrix( void ) const
	{
		return _projection;
	}

	void set_projection( const matrix4 &m )
	{
		_projection = m;
	}

	matrix4 current_matrix( void ) const
	{
		return  model_matrix() * view_matrix() * projection_matrix();
	}

	/// @brief Restore a saved matrix.
	void restore_matrix( void );


	/// @brief Construct a new shader.
	/// @tparams Args Constructor arguments for shader.
	/// @sa gl::shader::shader
	template<typename ...Args>
	std::shared_ptr<shader> new_shader( Args &&...args )
	{
		return std::make_shared<shader>( std::forward<Args>( args )... );
	}

	/// @brief Construct a new vertex shader.
	/// @tparams Args Constructor arguments for vertex shader.
	/// @sa gl::shader::shader
	template<typename ...Args>
	std::shared_ptr<shader> new_vertex_shader( Args &&...args )
	{
		return std::make_shared<shader>( gl::shader::type::VERTEX, std::forward<Args>( args )... );
	}

	/// @brief Construct a new fragment shader
	/// @tparams Args Constructor arguments for fragment shader.
	/// @sa gl::shader::shader
	template<typename ...Args>
	std::shared_ptr<shader> new_fragment_shader( Args &&...args )
	{
		return std::make_shared<shader>( gl::shader::type::FRAGMENT, std::forward<Args>( args )... );
	}

	/// @brief Construct a new program
	/// @tparams Args Constructor arguments for program.
	/// @sa gl::program::program
	template<typename ...Args>
	std::shared_ptr<program> new_program( Args &&...args )
	{
		return std::make_shared<program>( std::forward<Args>( args )... );
	}

	/// @brief Construct a new texture
	/// @tparams Args Constructor arguments for texture.
	/// @sa gl::texture::texture
	template<typename ...Args>
	std::shared_ptr<texture> new_texture( Args &&...args )
	{
		return std::make_shared<texture>( std::forward<Args>( args )... );
	}

	/// @brief Construct a new vertex buffer
	std::shared_ptr<vertex_buffer> new_vertex_buffer( void )
	{
		return std::make_shared<vertex_buffer>();
	}

	/// @brief Construct a new vertex buffer
	std::shared_ptr<vertex_buffer> new_vertex_buffer( const float *data, size_t n, buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		return std::make_shared<vertex_buffer>( data, n, u );
	}

	/// @brief Construct a new vertex buffer
	std::shared_ptr<vertex_buffer> new_vertex_buffer( const std::vector<float> &data, buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		return std::make_shared<vertex_buffer>( data, u );
	}

	/// @brief Construct a new element buffer
	template<typename ...Args>
	std::shared_ptr<element_buffer> new_element_buffer( Args &&...args )
	{
		return std::make_shared<element_buffer>( std::forward<Args>( args )... );
	}

	/// @brief Construct a new vertex array
	template<typename ...Args>
	std::shared_ptr<vertex_array> new_vertex_array( Args &&...args )
	{
		return std::make_shared<vertex_array>( std::forward<Args>( args )... );
	}

	void reset( void );

	/// @brief Setup debugging (if available).
	void setup_debugging( void );

private:
	base::irect _viewport;
	std::vector<matrix4> _matrix;
	matrix4 _view;
	matrix4 _projection;
	std::vector<base::irect> _scissors;
};

////////////////////////////////////////

}

