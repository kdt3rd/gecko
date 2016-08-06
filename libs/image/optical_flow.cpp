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

#include "optical_flow.h"

////////////////////////////////////////

namespace
{
using namespace image;

static plane extract_u( const vector_field &v )
{
	return v.u();
}

////////////////////////////////////////

static plane extract_v( const vector_field &v )
{
	return v.v();
}

} // empty namespace

////////////////////////////////////////

namespace image
{

void
add_oflow( engine::registry &r )
{
	using namespace engine;

	r.register_constant<image::vector_field>();
	r.add( op( "v.extract_u", extract_u, op::simple ) );
	r.add( op( "v.extract_v", extract_v, op::simple ) );

	add_vector_ops( r );
	add_patchmatch( r );
}

////////////////////////////////////////

} // namespace image

