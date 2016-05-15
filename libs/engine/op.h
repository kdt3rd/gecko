//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <array>
#include <memory>
#include <functional>
#include <typeinfo>
#include <base/const_string.h>
#include <base/contract.h>

////////////////////////////////////////

namespace engine
{

#if 0
plane read( const std::string &fn, int p )
{
}

void add( scanline &dest, int y, const plane &a, const plane &b );

std::vector<float> createKernel( const constant<size_t> &width, const constant<kernel_type> &type );

void convolveH( scanline &dest, int y, const plane &a, const constant<std::vector<float>> &k );


template <typename T>
class constant
{
	typedef T value_type;
	constant( value_type && a )
	{
		_node = dag::create_constant( std::forward<value_type>( a ) );
	}

	constant( graph &g, value_type && a )
	{
		_node = g.create_constant( std::forward<value_type>( a ) );
	}

	constant( const std::function<value_type(void)> &f )
	{
		_node = dag::defer_constant( f );
	}

	constant( graph &g, const std::function<value_type(void)> &f )
	{
		_node = g.defer_constant( f );
	}	

	void update_dag( dag &newdag )
	{
		_node = newdag.move_constant( _node );
	}
	
};
class plane
{
public:
	template <typename Func, typename... Args>
	plane( Func&& f, Args&&... args )
	{
		_node = dag::add_function( std::forward<Func>( f ), std::forward<Args>( args )... );
	}

	template <typename... FuncArgs, typename... Args>
	plane( plane_info &i, void (*func)( scanline &, int, FuncArgs... ), Args &&... args )
	{
	}

	/// Non-multithread routine
	template <typename... FuncArgs, typename... Args>
	plane( plane_info &i, void (*func)( buffer &, FuncArgs... ), Args &&... args )
	{
	}

	const plane_info &info( void ) const { return _info; }

	scanline scanline( int y ) const
	{
		std::lock_guard<dag::scanline_lock> lk( lock( y ) );

		if ( ! _buf )
		{
			if ( _node.type() == typeid(scanline) )
			{
				if ( ! _node.want_cache() )
					return _node.evaluate( y );

				_buf = buffer( info() );
				_node.fill( _buf.ref_scanline( y ), y );
			}
			else
				_buf = _node.evaluate();
		}
		else if ( ! complete( y ) )
		{
			precondition( _node.type() == typeid(scanline), "Expecting a scanline op for a partial buffer" );
			_node.fill( _buf.ref_scanline( y ), y );
		}

		return _buf.const_ref_scanline( y );
	}

	buffer data( void ) const
	{
		if ( ! _buf )
			_buf = _node.evaluate();
		return _buf;
	}
};

void foo()
{
	image i = read( "foo.tiff" );

	plane lum = ( i[0] + i[1] + i[2] ) / 3;

	std::cout << "Average: " << ave( lum ) << std::endl;
	write( "mono.tiff", image( lum ) );
}
#endif

/// @brief base class for function storage used by op.
///
/// An operator function has some attributes that will be queried in
/// addition to the function being called.
class op_function
{
public:
	virtual ~op_function( void );

	virtual const std::type_info &result_type( void ) const = 0;
	virtual size_t input_size( void ) const = 0;
};

///
/// It is best to support multiple types of processing to minimize
/// memory, maximize cache locality, and yet still enable global
/// operations like optical flow to be part of the system and
/// scheduled as such.
///
/// Desires:
/// - graph of registered operations for global optimization
///   - minimize global memory use / maximal re-use
///     - scanline ops should be grouped somehow
///   - rewrite engine (operation collapse / reorder)
///   - part of this needs to know when an operation is a scanline op,
///     so the memory computation is lessened
///   - the artifacts of creating a graph should be hidden in code that looks
///     like images / planes / values
///     - probably need graph merging
///   - metadata / image information should flow as the graph is constructed
///     - means that inputs that are only used as reference aren't counted
///     - output dimensions are known at every step
///
/// - execution of the graph needs to be a push/pull system?
///   - scanline ops prefer to pull
///   - global ops it's probably better to externally order their inputs for memory
///
/// TODO:
/// - figure out how to have frame-to-frame re-use flagged for cache
///   (based on memory available)
///
class op
{
public:
	/// One-to-one parallel, meaning the operation only needs data in
	/// one dimension at a time, and does not need any other data
	/// elements other than that dimension (no area / neighborhood
	/// outside of the single dimension being split upon). So a
	/// horizontal filter of a scanline separable convolution would be
	/// fine, but the vertical not. The prime example of this are the
	/// simple math operations.
	struct one_to_one_parallel_t {};
	/// N-to-one parallel, similar to one-to-one, but this would be
	/// for an operation such as a scanline implementation of a
	/// vertical filter. The processing needs scanlines, but + / -
	/// scanlines from the current scanline.
	struct n_to_one_parallel_t {};

	/// Multi-threaded op, where it operates using multiple threads,
	/// but may randomly access data from the input, or some other
	/// reason it isn't trivially parallel
	struct threaded_t {};

	/// Single threaded op, where an operation that has to modify the
	/// buffer all at once. Floyd-Steinberg dithering is a sample of
	/// this. If the engine encounters several of these in the
	/// processing graph which have no missing dependencies, it may
	/// run them in parallel
	struct single_threaded_t {};

	/// Stronger form of a single threaded op, where not only is it
	/// single threaded, but additionally, only one of these can run
	/// at a time, disallowing any other parallel engine tasks
	struct solitary_t {};

	/// a placeholder op to store a particular value type, this has no
	/// processing
	struct value_t {};

	/// tag to specify an op is fully one-to-one (i.e. scanline)
	/// parallel when registering
	static constexpr one_to_one_parallel_t one_to_one {};
	/// tag to specify an op is fully parallel but needs surrounding
	/// data on read when registering
	static constexpr n_to_one_parallel_t n_to_one {};
	/// tag to specify an op is multi threaded when registering
	static constexpr threaded_t threaded {};
	/// tag to specify an op is single threaded when registering
	static constexpr single_threaded_t single_threaded {};
	/// tag to specify an op is single threaded and solitary
	static constexpr solitary_t solitary {};
	/// tag to specify an op is a value
	static constexpr value_t value {};

	/// Enum class to store the operation type for later logic
	enum class style
	{
		ONE_TO_ONE,
		N_TO_ONE,
		MULTI_THREADED,
		SINGLE_THREADED,
		SOLITARY,
		VALUE
	};

	/// @brief Construct an op that implements one-to-one processing in parallel.
	///
	/// These should be grouped together by the processing graph and
	/// processed a one-to-one group at a time, parallelizing on the
	/// other axes.
	template <typename Functor, typename GroupProcessFunc>
	op( base::cstring n, Functor f, GroupProcessFunc g, one_to_one_parallel_t );
	/// @brief Construct an op that does n-to-one processing.
	///
	/// This should be allowed to be grouped with one-to-one, at least
	/// at the beginning of the group.
	template <typename Functor, typename GroupProcessFunc>
	op( base::cstring n, Functor f, GroupProcessFunc g, n_to_one_parallel_t );
	/// TODO: can DimensionDividerFunc be a specific function type?
	/// i.e. void (*)( const dim3 &opDims, std::vector<std::pair<int,int>> &threadRanges )
	/// where threadRanges is pre-sized to have the number of threads?
	/// Construct an op that is multi-threaded
	template <typename Functor, typename DimensionDividerFunc>
	op( base::cstring n, Functor f, DimensionDividerFunc pt, threaded_t );
	/// Construct an op that is single threaded
	template <typename Functor>
	op( base::cstring n, Functor f, single_threaded_t );
	/// Construct an op that is single threaded and solitary
	template <typename Functor>
	op( base::cstring n, Functor f, solitary_t );
	/// Construct an op that is a value placeholder
	op( base::cstring n, const std::reference_wrapper<const std::type_info> &ti, value_t );

	op( void ) = default;
	op( const op & ) = default;
	op( op && ) = default;
	op &operator=( const op & ) = default;
	op &operator=( op && ) = default;
	~op( void );

	inline const std::string &name( void ) const;

	inline style processing_style( void ) const;

	inline size_t input_size( void ) const;

	inline op_function &function( void ) const;

private:
	std::string _name;
	std::shared_ptr<op_function> _func;
	style _style;
};

////////////////////////////////////////

inline const std::string &op::name( void ) const
{
	return _name;
}

////////////////////////////////////////

inline op::style op::processing_style( void ) const
{
	return _style;
}

////////////////////////////////////////

inline size_t op::input_size( void ) const
{
	precondition( _func, "Invalid operation function for operation {0}", _name );
	return _func->input_size();
}

////////////////////////////////////////

inline op_function &op::function( void ) const
{
	precondition( _func, "Invalid operation function for operation {0}", _name );
	return (*_func);
}

} // namespace engine



