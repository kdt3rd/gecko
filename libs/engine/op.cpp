//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "op.h"
#include <functional>

////////////////////////////////////////

namespace
{
using namespace engine;

class null_func : public op_function
{
public:
	null_func( const std::reference_wrapper<const std::type_info> &ti );
	virtual ~null_func( void ) override;

	virtual const std::type_info &result_type( void ) const override;

	virtual size_t input_size( void ) const override;
	virtual const std::type_info &input_type( size_t ) const override
	{
		return typeid(void);
	}


	virtual any process( graph &, const dimensions &, const std::vector<any> & ) const override;

	virtual any create_value( const dimensions & ) const override
	{
		return any();
	}

	virtual std::shared_ptr<subgroup_function> create_group_function( void ) const override
	{
		throw_runtime( "attempt to create group fucntoin on a null function" );
	}
	virtual void dispatch_group( subgroup &, const dimensions & ) const override
	{
		throw_runtime( "attempt to dispatch group on a null function" );
	}
private:
	std::reference_wrapper<const std::type_info> _info;
};

////////////////////////////////////////

null_func::null_func( const std::reference_wrapper<const std::type_info> &ti )
	: _info( ti )
{
}

////////////////////////////////////////

null_func::~null_func( void )
{
}

////////////////////////////////////////

const std::type_info &
null_func::result_type( void ) const
{
	return _info.get();
}

////////////////////////////////////////

size_t
null_func::input_size( void ) const
{
	return 0;
}

////////////////////////////////////////

any
null_func::process( graph &, const dimensions &, const std::vector<any> & ) const
{
	return any();
}

}

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

op_function::~op_function( void ) noexcept
{
}

////////////////////////////////////////

op::op( base::cstring n, const std::reference_wrapper<const std::type_info> &ti, value_t )
	: _name( n ), _func( new null_func( ti ) ), _style( style::VALUE )
{
}

////////////////////////////////////////

op::~op( void )
{
}

////////////////////////////////////////

} // engine
