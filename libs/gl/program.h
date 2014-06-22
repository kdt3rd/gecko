
#pragma once

#include <memory>
#include <base/color.h>
#include <base/point.h>
#include <base/size.h>
#include "shader.h"
#include "matrix4.h"
#include "enums.h"

namespace gl
{

class context;

////////////////////////////////////////

class program
{
public:
	typedef GLuint uniform;
	typedef GLuint attribute;

	program( const program &program ) = delete;

	program( void );
	program( const std::shared_ptr<shader> &vertex );
	program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment );
	program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment, const std::shared_ptr<shader> &geometry );

	~program( void );

	void attach( const std::shared_ptr<shader> &s );

	void link( void );

	std::string log( void );

	attribute get_attribute_location( const std::string &name );
	uniform get_uniform_location( const std::string &name );

	template <typename T>
	void set_uniform( const std::string &name, const T &value )
	{
		set_uniform( get_uniform_location( name ), value );
	}

	template <typename T>
	void set_uniform( const std::string &name, const T *values, size_t count )
	{
		set_uniform( get_uniform_location( name ), values, count );
	}

	void set_uniform( uniform uniform, int value );
	void set_uniform( uniform uniform, float value );
	void set_uniform( uniform uniform, const matrix4 &value );
	void set_uniform( uniform uniform, const base::color &value );
	void set_uniform( uniform uniform, const base::point &value );
	void set_uniform( uniform uniform, const base::size &value );

	size_t number_active_uniforms( void );
	std::pair<uniform_type,std::string> active_uniform( size_t i );

private:
	friend class context;

	GLuint _program;
};

////////////////////////////////////////

}



