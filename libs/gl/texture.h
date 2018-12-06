//
// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include "opengl.h"
#include "enums.h"
#include <limits>
#include "color.h"
#include <base/contract.h>

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL texture
class texture
{
public:
	enum class target
	{
		IMAGE_2D = GL_TEXTURE_2D,
		RECTANGLE = GL_TEXTURE_RECTANGLE,
		CUBE_MAP = GL_TEXTURE_CUBE_MAP,
	};

	/// @brief A bound OpenGL texture.
	class binding
	{
	public:
		/// @brief Copy constructor not allowed.
		binding( const binding &other ) = delete;

		/// @brief Move constructor.
		binding( binding &&other );

		/// @brief Default constructor.
		binding( void );

		/// @brief Construct binding from pre-existing OpenGL id.
		binding( GLuint txt, size_t unit, GLenum target );

		/// @brief Destructor.
		~binding( void );

		/// @brief Specify a single channel 2D image.
		void image_2d_red( format f, size_t w, size_t h, image_type type, const void *data, size_t stride_bytes = 0, bool needswap = false )
		{
			upload_helper( f, GL_RED, 1, w, h, type, data, stride_bytes, needswap );
		}

		/// @brief Specify a two channel 2D image.
		void image_2d_rg( format f, size_t w, size_t h, image_type type, const void *data, size_t stride_bytes = 0, bool needswap = false )
		{
			upload_helper( f, GL_RG, 2, w, h, type, data, stride_bytes, needswap );
		}
		
		/// @brief Specify a RGB 2D image.
		void image_2d_rgb( format f, size_t w, size_t h, image_type type, const void *data, size_t stride_bytes = 0, bool needswap = false )
		{
			upload_helper( f, GL_RGB, 3, w, h, type, data, stride_bytes, needswap );
		}

		/// @brief Specify a RGBA 2D image.
		void image_2d_rgba( format f, size_t w, size_t h, image_type type, const void *data, size_t stride_bytes = 0, bool needswap = false )
		{
			upload_helper( f, GL_RGBA, 4, w, h, type, data, stride_bytes, needswap );
		}

		/// @brief Update a 2D region of the image.
		void subimage_2d( format f, int x, int y, size_t w, size_t h, image_type type, const void *data, size_t stride_bytes = 0, bool needswap = false );

		/// @brief Set the texture wrapping mode.
		void set_wrapping( wrapping w );

		/// @brief Set the texture wrapping mode.
		void set_wrapping( wrapping s, wrapping t );

		/// @brief Set the scaling filters.
		void set_filters( filter min, filter mag );

		/// @brief Set the texture border color.
		void set_border_color( const color &c );

		/// @brief Swizzle the channels.
		void set_swizzle( swizzle r, swizzle g, swizzle b, swizzle a = swizzle::ONE );

		/// @brief Move operator.
		void operator=( binding &&other );

		inline size_t unit( void ) const { return _unit; }
	private:
		void upload_helper( format f, GLenum fmt, int c, size_t w, size_t h, image_type type, const void *data, size_t stride_bytes, bool needswap );

		size_t _unit;
		GLenum _target;
	};

	/// @brief Copying not allowed.
	texture( const texture &other ) = delete;

	/// @brief Construct a new texture.
	texture( target t = target::RECTANGLE )
		: _target( static_cast<GLenum>( t ) )
	{
		glGenTextures( 1, &_texture );
	}

	/// @brief Construct from the OpenGL ID.
	texture( GLuint t, target targ )
		: _texture( t ), _target( static_cast<GLenum>( targ ) )
	{
	}

	/// @brief Move constructor.
	texture( texture &&other )
		: _texture( other._texture ), _target( other._target )
	{
		other._texture = 0;
	}

	/// @brief Destructor.
	~texture( void )
	{
		if ( _texture > 0 )
			glDeleteTextures( 1, &_texture );
	}

	/// @brief Check if the texture is valid.
	inline bool is_valid( void ) const
	{
		return _texture > 0;
	}

	/// @brief Bind the texture to use it.
	inline binding bind( size_t unit = 0 )
	{
		precondition( is_valid(), "invalid texture" );
		return binding( _texture, unit, _target );
	}

	/// @brief OpenGL ID for the texture.
	GLuint id( void ) const
	{
		precondition( is_valid(), "invalid texture" );
		return _texture;
	}

private:
	GLuint _texture = 0;
	GLenum _target = 0;
};

////////////////////////////////////////

}

