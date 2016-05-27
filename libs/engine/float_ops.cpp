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

#include "float_ops.h"
#include "registry.h"

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

void
register_float_ops( registry &r )
{
	r.add( op( "d.castto", [](float v) -> double { return static_cast<double>( v ); }, op::simple ) );
	r.add( op( "d.castfrom", [](double v) -> float { return static_cast<float>( v ); }, op::simple ) );

	r.add( op( "f.negate", [](float v) -> float { return -v; }, op::simple ) );
	r.add( op( "d.negate", [](double v) -> double { return -v; }, op::simple ) );

	r.add( op( "f.add", [](float a, float b) -> float { return a + b; }, op::simple ) );
	r.add( op( "d.add", [](double a, double b) -> double { return a + b; }, op::simple ) );

	r.add( op( "f.sub", [](float a, float b) -> float { return a - b; }, op::simple ) );
	r.add( op( "d.sub", [](double a, double b) -> double { return a - b; }, op::simple ) );

	r.add( op( "f.mul", [](float a, float b) -> float { return a * b; }, op::simple ) );
	r.add( op( "d.mul", [](double a, double b) -> double { return a * b; }, op::simple ) );

	r.add( op( "f.div", [](float a, float b) -> float { return a / b; }, op::simple ) );
	r.add( op( "d.div", [](double a, double b) -> double { return a / b; }, op::simple ) );
}

////////////////////////////////////////

} // engine



