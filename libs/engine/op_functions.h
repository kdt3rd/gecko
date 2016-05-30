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

#include <typeinfo>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <base/contract.h>
#include <base/meta.h>
#include <base/function_traits.h>
#include "types.h"

////////////////////////////////////////

namespace engine
{

class graph;
class subgroup;
class subgroup_function;

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
	virtual const std::type_info &input_type( size_t I ) const = 0;

	virtual any process( graph &g, const dimensions &d, const std::vector<any> &inputs ) const = 0;

	virtual any create_value( const dimensions &d ) const = 0;

	virtual std::shared_ptr<subgroup_function> create_group_function( void ) const = 0;
	virtual void dispatch_group( subgroup &s, const dimensions &d ) const = 0;
};

////////////////////////////////////////

/// @brief subclass for one-to-one operations
///
/// NB: expects to be grouped into subgroup for processing.
template <typename Functor, typename GroupFunc>
class opfunc_one_to_one : public op_function
{
public:
	typedef typename base::function_traits<Functor>::function process_func;
	typedef std::function<void(subgroup &, const dimensions &)> group_dispatch;

	opfunc_one_to_one( Functor f, const group_dispatch &g )
		: _p( base::to_function( f ) ), _dispatch( g )
	{
	}
	virtual ~opfunc_one_to_one( void ) noexcept
	{
	}

	virtual const std::type_info &result_type( void ) const override
	{
		return typeid(typename group_dispatch::result_type);
	}

	virtual size_t input_size( void ) const override
	{
		return base::function_traits<Functor>::arity - 1;
	}

	virtual const std::type_info &input_type( size_t i ) const override
	{
		return base::function_traits<Functor>::arg_type( i + 1 );
	}

	virtual any process( graph &g, const dimensions &d, const std::vector<any> &inputs ) const override
	{
		throw_runtime( "all calls should happen as part of a group" );
	}

	virtual any create_value( const dimensions &d ) const override
	{
		return create_result<typename GroupFunc::result_type>( d );
	}

	virtual std::shared_ptr<subgroup_function> create_group_function( void ) const override
	{
		return std::make_shared<GroupFunc>( _p );
	}

	virtual void dispatch_group( subgroup &s, const dimensions &d ) const override
	{
		_dispatch( s, d );
	}

private:
	template <typename R, typename... Ctors>
	static inline R create_result( Ctors &&... c )
	{
		return R( std::forward<Ctors>( c )... );
	}

	process_func _p;
	group_dispatch _dispatch;
};

/// @brief subclass for simple thread / simple things
template <typename Functor>
class opfunc_simple : public op_function
{
public:
	typedef typename base::function_traits<Functor>::function process_func;

	opfunc_simple( Functor f )
		: _p( base::to_function( f ) )
	{
	}
	virtual ~opfunc_simple( void ) noexcept
	{
	}

	virtual const std::type_info &result_type( void ) const override
	{
		return typeid(typename process_func::result_type);
	}

	virtual size_t input_size( void ) const override
	{
		return base::function_traits<Functor>::arity;
	}

	virtual const std::type_info &input_type( size_t i ) const override
	{
		return base::function_traits<Functor>::arg_type( i );
	}

	virtual any process( graph &, const dimensions &, const std::vector<any> &inputs ) const override
	{
		return dispatch( inputs, base::gen_sequence<base::function_traits<Functor>::arity>{} );
	}


	virtual any create_value( const dimensions &d ) const override
	{
		return any();
	}

	virtual std::shared_ptr<subgroup_function> create_group_function( void ) const override
	{
		throw_runtime( "attempt to create group function on a simple thing" );
	}
	virtual void dispatch_group( subgroup &s, const dimensions &d ) const override
	{
		throw_runtime( "attempt to dispatch group on a simple thing" );
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

/// @brief for solitary things where this op needs to be locked from
/// any other thread doing other ops
template <typename Functor>
class opfunc_locked_simple : public opfunc_simple<Functor>
{
public:
	opfunc_locked_simple( Functor f )
		: opfunc_simple<Functor>( f )
	{
	}
	virtual ~opfunc_locked_simple( void ) noexcept
	{
	}

	virtual any process( graph &, const dimensions &, const std::vector<any> &inputs ) const override
	{
		std::lock_guard<std::mutex> lk( _mutex );
		return dispatch( inputs, base::gen_sequence<base::function_traits<Functor>::arity>{} );
	}

private:
	std::mutex _mutex;
};

} // namespace engine



