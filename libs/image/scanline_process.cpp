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

#include "scanline_process.h"
#include "scanline_group.h"
#include "threading.h"

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

static void
scanline_thread_process( size_t, int start, int end, engine::subgroup &sg, int w, int h )
{
	// the recursive process allows scanline to be re-used as
	// source and destination, iterative trivially avoids
	// recompute of branch-outs. if we add a scanline_group thing,
	// we can look for a scanline on the inputs to re-use or make
	// a new one if those are all refs to inputs, and get the best
	// of both worlds for the middle, and if we tell the scanline
	// group about the outputs, we can use those scanlines too, we
	// just have to check and make sure to use the appropriate
	// output scanline at the output node
	// create the output planes
	std::vector<std::shared_ptr<engine::subgroup_function>> funcs;
	sg.bind_functions( funcs );
	size_t nOuts = sg.outputs().size();
	scanline_group scans( w, nOuts );

	for ( int y = start; y < end; ++y )
	{
		// HRM, we can only really support this if there is 1 output
		// any more than that, and a particular output scanline may
		// be in use along a different path when the current path
		// reaches the current output
		// TODO: research a way to prevent that
		for ( size_t i = 0; i < nOuts; ++i )
			scans.output_scan( i, scan_ref( base::any_cast<plane &>( sg.output_val( i ) ), y ) );

		for ( size_t i = 0; i < sg.size(); ++i )
		{
			scanline_plane_functor &cur = static_cast<scanline_plane_functor &>( *(funcs[i]) );

			cur.update_inputs( y ); // for any inputs that are a reference to a plane

			scanline dest;
			if ( cur.is_output() )
				dest = scans.output_scan_and_clear( cur.output_index() );
			else
				dest = scans.find_or_checkout( cur.inputs() );

			cur.call( dest );

			// release the scanlines for the next iteration
			cur.deref_inputs();

			for ( auto &o: cur.outputs() )
				static_cast<scanline_plane_functor *>( o.first )->set_input( o.second, dest );
		}

		// don't really need this as we will just overwrite the index
		// the next loop
//		sg.deref_outputs();
	}
}

void dispatch_scan_processing( engine::subgroup &sg, const engine::dimensions &dims )
{
	int w = static_cast<int>( dims.x );
	int h = static_cast<int>( dims.y );

	threading::get().dispatch( std::bind( scanline_thread_process, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( sg ), w, h ), 0, h );
}


} // image



