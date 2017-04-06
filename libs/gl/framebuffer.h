//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "texture.h"

namespace gl
{

////////////////////////////////////////

/// @brief OpenGL framebuffer.
class framebuffer
{
public:
	class binding
	{
	public:
		binding( void )
		{
		}

		binding( const binding &other ) = delete;

		binding( binding &&other )
			: _fb( other._fb )
		{
			other._fb = 0;
		}

		~binding( void )
		{
			if ( _fb > 0 )
				unbind();
		}

		void attach( const texture &t )
		{
			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.id(), 0 );
		}

	private:
		friend class framebuffer;

		binding( GLuint fb )
			: _fb( fb )
		{
			_stack.push_back( this );
			glBindFramebuffer( GL_FRAMEBUFFER, _fb );
		}

		void rebind( void )
		{
			precondition( !_stack.empty(), "inconsistent stack state" );
			precondition( _stack.back() == this, "rebind out of sequence" );
			glBindFramebuffer( GL_FRAMEBUFFER, _fb );
		}

		void unbind( void )
		{
			precondition( !_stack.empty(), "inconsistent stack state" );
			precondition( _stack.back() == this, "framebuffer destroyed out of sequence" );
			_stack.pop_back();
			if ( _stack.empty() )
				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			else
				_stack.back()->rebind();
		}

		GLuint _fb = 0;

		static std::vector<binding *> _stack;
	};

	framebuffer( void );

	framebuffer( GLuint fb );

	framebuffer( framebuffer &&other );

	~framebuffer( void );

	inline bool is_valid( void ) const
	{
		return _fb > 0;
	}

	binding bind( void )
	{
		return binding( _fb );
	}

private:
	GLuint _fb = 0;
};

////////////////////////////////////////

}

