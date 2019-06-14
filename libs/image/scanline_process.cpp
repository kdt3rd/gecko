// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "scanline_process.h"

#include "scanline_group.h"
#include "threading.h"

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

scanline_plane_functor::~scanline_plane_functor( void ) {}

////////////////////////////////////////

static void scanline_thread_process(
    size_t, int start, int end, engine::subgroup &sg, int offx, int w )
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
    size_t         nOuts = sg.outputs().size();
    scanline_group scans( offx, w, nOuts );

    for ( int y = start; y < end; ++y )
    {
        // HRM, we can only really support this if there is 1 output
        // any more than that, and a particular output scanline may
        // be in use along a different path when the current path
        // reaches the current output
        // TODO: research a way to prevent that
        for ( size_t i = 0; i < nOuts; ++i )
            scans.output_scan(
                i,
                scan_ref( base::any_cast<plane &>( sg.output_val( i ) ), y ) );

        for ( size_t i = 0; i < sg.size(); ++i )
        {
            scanline_plane_functor &cur =
                static_cast<scanline_plane_functor &>( *( funcs[i] ) );

            cur.update_inputs(
                y ); // for any inputs that are a reference to a plane

            scanline dest;
            if ( cur.is_output() )
                dest = scans.output_scan_and_clear( cur.output_index() );
            else
                dest = scans.find_or_checkout( cur.inputs(), cur.in_place() );

            //			if ( y == 0 )
            //			{
            //				std::cout << i << ": " << sg.gref().op_registry()[sg.gref()[sg.members()[i]].op()].name() << std::endl;
            //			}
            cur.call( dest, y );

            // release the scanlines for the next iteration
            cur.deref_inputs();

            for ( auto &o: cur.outputs() )
                static_cast<scanline_plane_functor *>( o.first )->set_input(
                    o.second, dest );
        }

        // don't really need this as we will just overwrite the index
        // the next loop
        //		sg.deref_outputs();
    }
}

void dispatch_scan_processing(
    engine::subgroup &sg, const engine::dimensions &dims )
{
    int w = static_cast<int>( dims.x2 - dims.x1 + 1 );
    int h = static_cast<int>( dims.y2 - dims.y1 + 1 );

    threading::get().dispatch(
        std::bind(
            scanline_thread_process,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( sg ),
            dims.x1,
            w ),
        dims.y1,
        h );
}

} // namespace image
