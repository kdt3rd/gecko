
#pragma once

#include "vector.h"
#include "color.h"
#include <base/meta.h>
#include <base/tuple_util.h>

namespace gl
{

////////////////////////////////////////

namespace detail
{
	template<typename T> struct count_stride {};

	template<> struct count_stride<float>
	{
		enum { value = 1 };
	};

	template<> struct count_stride<vec2>
	{
		enum { value = 2 };
	};

	template<> struct count_stride<vec3>
	{
		enum { value = 3 };
	};

	template<> struct count_stride<vec4>
	{
		enum { value = 4 };
	};

	template<> struct count_stride<color>
	{
		enum { value = 4 };
	};
}

////////////////////////////////////////

/// @brief Buffer data is an array of vertex attributes
template <typename ...Args>
class vertex_buffer_data
{
public:
	typedef std::tuple<Args...> tuple;

	vertex_buffer_data( void )
	{
	}

	vertex_buffer_data( std::initializer_list<tuple> l )
	{
		for( const auto &t: l )
			push_back( t );
	}

	/// @brief Add attributes
	void push_back( const Args &...args )
	{
		precondition( !_vbo, "cannot add attributes after creating VBO" );
		append( args... );
	}

	/// @brief Add attributes
	void push_back( const tuple &t )
	{
		precondition( !_vbo, "cannot add attributes after creating VBO" );
		auto func = [=]( const Args &...args ) { this->push_back( args... ); };
		base::apply( func, t );
	}

	void pop_back( void )
	{
		precondition( !_vbo, "cannot add attributes after creating VBO" );
		_data.erase( _data.end() - static_cast<std::vector<float>::difference_type>( stride() ), _data.end() );
	}

	size_t size( void ) const
	{
		return _data.size();
	}

	bool empty( void ) const
	{
		return _data.empty();
	}

	void clear( void )
	{
		_data.clear();
	}

	const float *data( void ) const
	{
		return _data.data();
	}

	float operator[]( size_t i ) const
	{
		return _data[i];
	}

	size_t stride( void ) const
	{
		return attribute_offset( sizeof...(Args) );
	}

	size_t attribute_offset( size_t a ) const
	{
		static const constexpr std::array<size_t,sizeof...(Args)> sizes = {{ detail::count_stride<Args>::value... }};
		size_t sum = 0;
		for ( size_t i = 0; i < a; ++i )
			sum += sizes.at( i );
		return sum;
	}

	size_t attribute_size( size_t i ) const
	{
		static const constexpr std::array<size_t,sizeof...(Args)> sizes = {{ detail::count_stride<Args>::value... }};
		return sizes.at( i );
	}

	size_t attributes( void ) const
	{
		return sizeof...( Args );
	}

	std::shared_ptr<vertex_buffer> vbo( buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		if ( !_vbo )
			_vbo = std::make_shared<vertex_buffer>( _data, u );
		return _vbo;
	}

private:
	template<typename ...More>
	void append( const vec2 &p, More ...args )
	{
		_data.push_back( p[0] );
		_data.push_back( p[1] );
		append( args... );
	}

	template<typename ...More>
	void append( const vec3 &p, More ...args )
	{
		_data.push_back( p[0] );
		_data.push_back( p[1] );
		_data.push_back( p[2] );
		append( args... );
	}

	template<typename ...More>
	void append( const vec4 &p, More ...args )
	{
		_data.push_back( p[0] );
		_data.push_back( p[1] );
		_data.push_back( p[2] );
		_data.push_back( p[3] );
		append( args... );
	}

	template<typename ...More>
	void append( const color &c, More ...args )
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

	void append( void )
	{
	}

	std::shared_ptr<vertex_buffer> _vbo;
	std::vector<float> _data;
};

////////////////////////////////////////

}
