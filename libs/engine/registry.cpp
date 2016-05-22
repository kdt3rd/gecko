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

#include "registry.h"
#include <base/contract.h>
#include <limits>

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

registry::registry( void )
{
	register_constant<float>();
	register_constant<double>();
	register_constant<uint8_t>();
	register_constant<uint16_t>();
	register_constant<uint32_t>();
	register_constant<uint64_t>();
	register_constant<int8_t>();
	register_constant<int16_t>();
	register_constant<int32_t>();
	register_constant<int64_t>();
	register_constant<std::string>();
}

////////////////////////////////////////

registry::~registry( void )
{
}

////////////////////////////////////////

op_id
registry::find( const base::cstring &n ) const
{
	auto i = _name_to_op.find( n );
	if ( i == _name_to_op.end() )
		throw_logic( "No operation by name of {0} found", n );

	return i->second;
}

////////////////////////////////////////

op_id
registry::find_constant( const std::type_info &ti ) const
{
	// name is not guaranteed to be unique, but neither is hash_code,
	// but should be good enough for the provided types
	return find( ti.name() );
}

////////////////////////////////////////

op_id
registry::add( const op &o )
{
	op_id r = static_cast<op_id>( _ops.size() );
	precondition( (_ops.size() + 1) < std::numeric_limits<op_id>::max(), "Too many operations registered for size of op_id" );
	if ( _name_to_op.find( o.name() ) != _name_to_op.end() )
		throw_logic( "Operation by the name of {0} already registered", o.name() );

	_name_to_op[o.name()] = r;
	_ops.push_back( o );
	return r;
}

////////////////////////////////////////

op_id
registry::add( op &&o )
{
	op_id r = static_cast<op_id>( _ops.size() );
	precondition( (_ops.size() + 1) < std::numeric_limits<op_id>::max(), "Too many operations registered for size of op_id" );
	if ( _name_to_op.find( o.name() ) != _name_to_op.end() )
		throw_logic( "Operation by the name of {0} already registered", o.name() );

	_name_to_op[o.name()] = r;
	_ops.emplace_back( std::move( o ) );
	return r;
}

////////////////////////////////////////

const registry &registry::pod_registry( void )
{
	static registry pods;
	return pods;
}

////////////////////////////////////////

} // engine



