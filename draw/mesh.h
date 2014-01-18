
#pragma once

#include <vector>
#include <array>
#include <core/meta.h>
#include <core/point.h>
#include <core/color.h>
#include <gl/context.h>
#include <gl/vertex_array.h>

namespace draw
{

////////////////////////////////////////

namespace detail
{
	template<typename T> struct count_components {};

	template<> struct count_components<core::point>
	{
		enum { value = 2 };
	};

	template<> struct count_components<core::color>
	{
		enum { value = 4 };
	};

	template<> struct count_components<float>
	{
		enum { value = 1 };
	};

	template<size_t N> struct count_components<float[N]>
	{
		enum { value = N };
	};
}


////////////////////////////////////////

template <typename ...Args>
class mesh
{
public:
	void begin( gl::primitive prim )
	{
		_parts.emplace_back( prim, _data.size() / number_components(), 0 );
	}

	void end( void )
	{
		precondition( !_parts.empty(), "no part to end" );
		size_t points = ( _data.size() / number_components() ) - std::get<1>( _parts.back() );
		std::get<2>( _parts.back() ) = points;
	}

	void push_back( const Args &...args )
	{
		append( args... );
	}

	template<typename ...Names>
	void set_attrib_pointers( gl::context &GL, std::shared_ptr<gl::vertex_array> &vao, Names ...namelist ) const
	{
		static_assert( sizeof...(Names) == sizeof...(Args), "invalid number of program attributes" );
		auto buf = GL.new_buffer<float>( gl::buffer<float>::target::ARRAY_BUFFER, _data, gl::usage::STATIC_DRAW );
		auto va = vao->bind();

		std::array<size_t,sizeof...(Args)> components = {{ detail::count_components<Args>::value... }};
		std::array<gl::program::attribute,sizeof...(Args)> names = {{ static_cast<gl::program::attribute>( namelist )... }};

		size_t stride = number_components();
		size_t offset = 0;

		for ( size_t i = 0; i < sizeof...(Args); ++i )
		{
			va.attrib_pointer( names[i], buf, components[i], stride, offset );
			offset += components[i];
		}
	}

	const std::vector<std::tuple<gl::primitive,size_t,size_t>> &parts( void ) const
	{
		return _parts;
	}

private:

	inline size_t number_components( void ) const
	{
		return sum( static_cast<size_t>( detail::count_components<Args>::value )... );
	}

	template<typename ...More>
	void append( const core::point &p, More ...args )
	{
		_data.push_back( p.x() );
		_data.push_back( p.y() );
		append( args... );
	}

	template<typename ...More>
	void append( const core::color &c, More ...args )
	{
		_data.push_back( c.red() );
		_data.push_back( c.green() );
		_data.push_back( c.blue() );
		_data.push_back( c.alpha() );
		append( args... );
	}

	template<typename ...More>
	void append( float f, More ...args )
	{
		_data.push_back( f );
		append( args... );
	}

	template<size_t N, typename ...More>
	void append( float f[N], More ...args )
	{
		for ( size_t i = 0; i < N; ++i )
			_data.push_back( f[i] );
		append( args... );
	}

	void append( void )
	{
	}

	std::vector<float> _data;
	std::vector<std::tuple<gl::primitive,size_t,size_t>> _parts;
};

////////////////////////////////////////

}

