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
	virtual ~null_func( void );

	virtual const std::type_info &result_type( void ) const;

	virtual size_t input_size( void ) const;

	virtual any process( graph &, const dimensions &, const std::vector<any> & ) const;
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



