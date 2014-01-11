
#pragma once

#include <memory>
#include "shader.h"
#include "matrix4.h"

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
	void set_uniform( const std::string &name, const T *values, uint count )
	{
		set_uniform( get_uniform_location( name ), values, count );
	}

	void set_uniform( uniform uniform, int value );
	void set_uniform( uniform uniform, float value );
	void set_uniform( uniform uniform, const matrix4 &value );
	/*
	void set_uniform( const Uniform& uniform, const Vec2& value );
	void set_uniform( const Uniform& uniform, const Vec3& value );
	void set_uniform( const Uniform& uniform, const Vec4& value );
	void set_uniform( const Uniform& uniform, const float* values, uint count );
	void set_uniform( const Uniform& uniform, const Vec2* values, uint count );
	void set_uniform( const Uniform& uniform, const Vec3* values, uint count );
	void set_uniform( const Uniform& uniform, const Vec4* values, uint count );
	void set_uniform( const Uniform& uniform, const Mat3& value );
	void set_uniform( const Uniform& uniform, const Mat4& value );
	*/

	program( void );
	program( const std::shared_ptr<shader> &vertex );
	program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment );
	program( const std::shared_ptr<shader> &vertex, const std::shared_ptr<shader> &fragment, const std::shared_ptr<shader> &geometry );
private:
	friend class context;

	GLuint _program;
};

////////////////////////////////////////

}



