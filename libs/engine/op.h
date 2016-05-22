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
#include <vector>
#include <memory>
#include <functional>
#include <typeinfo>
#include <base/const_string.h>
#include <base/contract.h>
#include <base/variadic_function.h>
#include "types.h"

////////////////////////////////////////

namespace engine
{

class graph;

/// @brief base class for function storage used by op.
///
/// An operator function has some attributes that will be queried in
/// addition to the function being called.
class op_function
{
public:
	virtual ~op_function( void ) noexcept;

	virtual const std::type_info &result_type( void ) const = 0;
	virtual size_t input_size( void ) const = 0;

	virtual any process( graph &g, const dimensions &d, const std::vector<any> &inputs ) const = 0;
};

template <typename R, typename ...Args>
struct function_info
{
	static constexpr size_t arg_count = sizeof...(Args);
};

template <typename Functor, typename GroupFunc>
class opfunc_one_to_one : public op_function
{
public:
	typedef typename base::function_traits<Functor>::function process_func;
	typedef typename base::function_traits<GroupFunc>::function group_dispatch;

	opfunc_one_to_one( Functor f, GroupFunc g )
		: _p( base::to_function( f ) ), _dispatch( base::to_function( g ) )
	{
	}
	virtual ~opfunc_one_to_one( void ) noexcept
	{
	}

	virtual const std::type_info &result_type( void ) const
	{
		return typeid(typename group_dispatch::result_type);
	}

	virtual size_t input_size( void ) const
	{
		return base::function_traits<Functor>::arity - 1;
	}

	virtual any process( graph &g, const dimensions &d, const std::vector<any> &inputs ) const
	{
		return static_cast<any>( _dispatch( g, d, _p, inputs ) );
	}

private:
	process_func _p;
	group_dispatch _dispatch;
};

template <typename Functor>
class opfunc_single : public op_function
{
public:
	typedef typename base::function_traits<Functor>::function process_func;

	opfunc_single( Functor f )
		: _p( base::to_function( f ) )
	{
	}
	virtual ~opfunc_single( void ) noexcept
	{
	}

	virtual const std::type_info &result_type( void ) const
	{
		return typeid(typename process_func::result_type);
	}

	virtual size_t input_size( void ) const
	{
		return base::function_traits<Functor>::arity;
	}

	virtual any process( graph &g, const dimensions &d, const std::vector<any> &inputs ) const
	{
		return dispatch( inputs, base::gen_sequence<base::function_traits<Functor>::arity>{} );
	}

private:
	template <size_t I>
	inline typename base::function_traits<Functor>::template get_arg_type<I>::type
	extract( const std::vector<any> &inputs ) const
	{
		typedef typename base::function_traits<Functor>::template get_arg_type<I>::type ret_type;
		return engine::any_cast<ret_type>( inputs[I] );
	}

	template <size_t... S>
	inline any dispatch( const std::vector<any> &inputs, const base::sequence<S...> & ) const
	{
		return _p( extract<S>( inputs )... );
	}

	process_func _p;
};

////////////////////////////////////////

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
	inline op( base::cstring n, Functor f, GroupProcessFunc g, one_to_one_parallel_t )
			: _name( n ), _func( new opfunc_one_to_one<Functor, GroupProcessFunc>( f, g ) ), _style( style::ONE_TO_ONE )
	{
	}
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
	inline op( base::cstring n, Functor f, single_threaded_t )
		: _name( n ), _func( new opfunc_single<Functor>( f ) ),
		  _style( style::SINGLE_THREADED )
	{}

	/// Construct an op that is single threaded and solitary
	template <typename Functor>
	op( base::cstring n, Functor f, solitary_t )
		: _name( n ), _func( new opfunc_single<Functor>( f ) ),
		  _style( style::SOLITARY )
	{}

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



