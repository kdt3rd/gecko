
#pragma once

#include "vector.h"
#include "color.h"
#include <base/meta.h>
#include <base/tuple_util.h>
#include <utility>

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

/// @brief Buffer data is an array of vertex attributes.
/// The vertex_buffer_data holds an array ofvertex attributes.
/// An attribute can be position, normal, color, matrices, or
/// other information (used by the vertex shader).
/// @tparams Args Types of the per vertex attributes (float, vec2, vec3, vec4, matrix4, or color types).
template <typename ...Args>
class vertex_buffer_data
{
public:
	/// @brief Tuple of the attributes.
	/// Wrapper for a tuple.
	/// It is only used to create an initializer list.
	struct tuple
	{
		tuple( Args ...args )
			: data( std::forward<Args>( args )... )
		{
		}
		std::tuple<Args...> data;
	};

	/// @brief Default constructor.
	vertex_buffer_data( void )
	{
	}

	/// @brief Initializer list constructor.
	/// @param list List of tuples (one entry per vertex).
	vertex_buffer_data( std::initializer_list<tuple> list )
	{
		for( const auto &t: list )
			push_back( t );
	}

	/// @brief Add a vertex with the given attributes.
	/// @param args Attributes for the vertex.
	void push_back( const Args &...args )
	{
		precondition( !_vbo, "cannot add vertex after creating VBO" );
		append( args... );
	}

	/// @brief Add a vertex with the attribute tuple.
	/// @param t Tuple of attribute values for the vertex.
	void push_back( const std::tuple<Args...> &t )
	{
		precondition( !_vbo, "cannot add vertex after creating VBO" );
		auto func = [=]( const Args &...args ) { this->push_back( args... ); };
		base::apply( func, t );
	}

	/// @brief Remove last vertex added.
	void pop_back( void )
	{
		precondition( !_vbo, "cannot remove vertex after creating VBO" );
		_data.erase( _data.end() - static_cast<std::vector<float>::difference_type>( stride() ), _data.end() );
	}

	/// @brief Returns the number of vertices.
	size_t size( void ) const
	{
		return _data.size() / stride();
	}

	/// @brief Returns true if the buffer is empty.
	bool empty( void ) const
	{
		return _data.empty();
	}

	/// @brief Clear the buffer of all data.
	void clear( void )
	{
		_data.clear();
	}

	/// @brief The raw float data for all vertices.
	/// There will be size()*stride() floats.
	/// @returns Pointer to raw float data.
	const float *data( void ) const
	{
		return _data.data();
	}

	/// @brief Return the ith float.
	/// @param i Index of float to return.
	/// @returns ith float in array.
	float operator[]( size_t i ) const
	{
		return _data[i];
	}

	/// @brief Number of floats for one vertex.
	size_t stride( void ) const
	{
		return attribute_offset( sizeof...(Args) );
	}

	/// @brief Offset within a vertex for attribute a.
	/// @param a Attribute to calculate offset of.
	size_t attribute_offset( size_t a ) const
	{
		static const constexpr std::array<size_t,sizeof...(Args)> sizes = {{ detail::count_stride<Args>::value... }};
		size_t sum = 0;
		for ( size_t i = 0; i < a; ++i )
			sum += sizes.at( i );
		return sum;
	}

	/// @brief Number of floats used by attribute a.
	/// @param a Attribute to return the size of.
	/// @returns The number of floats for the attribute.
	size_t attribute_size( size_t a ) const
	{
		static const constexpr std::array<size_t,sizeof...(Args)> sizes = {{ detail::count_stride<Args>::value... }};
		return sizes.at( a );
	}

	/// @brief Number of attributes per vertex.
	/// @returns Number of attributes per vertex.
	size_t attributes( void ) const
	{
		return sizeof...( Args );
	}

	/// @brief Vertex buffer object for the data.
	/// The VBO is created as needed.
	/// Once a VBO is created, the data cannot be modified.
	/// @returns The vertex buffer object.
	std::shared_ptr<vertex_buffer> vbo( buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		if ( !_vbo )
			_vbo = std::make_shared<vertex_buffer>( _data, u );
		return _vbo;
	}

private:
	void push_back( const tuple &t )
	{
		push_back( t.data );
	}

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
