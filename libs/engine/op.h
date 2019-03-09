// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <base/const_string.h>
#include "op_functions.h"

////////////////////////////////////////

namespace engine
{

class subgroup;


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

	/// A simple op that does computation, but no memory allocation to
	/// optimize for, and only on (usually) pod types, so no threading
	/// operations of this type often have nulldim dimension
	struct simple_t {};

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

	/// tag to specify an op is simple (no threading, no memory allocations)
	static constexpr simple_t simple {};

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
		SIMPLE,
		VALUE
	};

	/// @brief Construct an op that implements one-to-one processing in parallel.
	///
	/// These should be grouped together by the processing graph and
	/// processed a one-to-one group at a time, parallelizing on the
	/// other axes.
	template <typename Functor, typename GroupProcessFunc>
	inline op( base::cstring n, Functor f, const GroupProcessFunc &, const std::function<void(subgroup &, const dimensions &)> &g, one_to_one_parallel_t )
			: _name( n ), _func( new opfunc_one_to_one<Functor, GroupProcessFunc>( f, g ) ), _style( style::ONE_TO_ONE )
	{
	}

	/// @brief Construct an op that does n-to-one processing.
	///
	/// This should be allowed to be grouped with one-to-one, at least
	/// at the beginning of the group.
	template <typename Functor, typename GroupProcessFunc>
	inline op( base::cstring n, Functor f, const GroupProcessFunc &, const std::function<void(subgroup &, const dimensions &)> &g, n_to_one_parallel_t )
		: _name( n ), _func( new opfunc_one_to_one<Functor, GroupProcessFunc, 2>( f, g ) ), _style( style::N_TO_ONE )
	{
	}

	/// TODO: should we provide some threading support here, or just
	/// leave it up to the library?  i.e. void (*)( const dim3
	/// &opDims, std::vector<std::pair<int,int>> &threadRanges ) where
	/// threadRanges is pre-sized to have the number of threads?
	/// Construct an op that is multi-threaded
	template <typename Functor>
	op( base::cstring n, Functor f, threaded_t )
		: _name( n ), _func( new opfunc_simple<Functor>( f ) ),
		  _style( style::MULTI_THREADED )
	{}

	/// Construct an op that is single threaded
	template <typename Functor>
	inline op( base::cstring n, Functor f, single_threaded_t )
		: _name( n ), _func( new opfunc_simple<Functor>( f ) ),
		  _style( style::SINGLE_THREADED )
	{}

	/// Construct an op that is single threaded and solitary
	template <typename Functor>
	op( base::cstring n, Functor f, solitary_t )
		: _name( n ), _func( new opfunc_locked_simple<Functor>( f ) ),
		  _style( style::SOLITARY )
	{}

	/// Construct an op that is trivial (i.e. math operation)
	template <typename Functor>
	op( base::cstring n, Functor f, simple_t )
		: _name( n ), _func( new opfunc_simple<Functor>( f ) ),
		  _style( style::SIMPLE )
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
	const std::type_info &input_type( size_t I ) const;

	inline bool can_group( const op &o ) const;

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

inline const std::type_info &op::input_type( size_t i ) const
{
	precondition( _func, "Invalid operation function for operation {0}", _name );
	return _func->input_type( i );
}

////////////////////////////////////////

inline bool op::can_group( const op &o ) const
{
	return ( processing_style() == style::ONE_TO_ONE &&
			 ( o.processing_style() == style::ONE_TO_ONE ||
			   o.processing_style() == style::N_TO_ONE ) &&
			 function().result_type() == o.function().result_type() );
}

////////////////////////////////////////

inline op_function &op::function( void ) const
{
	precondition( _func, "Invalid operation function for operation {0}", _name );
	return (*_func);
}

} // namespace engine



