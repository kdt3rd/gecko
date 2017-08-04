// Automatically generated from SVG
// (from file /home/iangodin/Sources/material-design-icons/toggle/svg/design/ic_check_box_outline_blank_24px.svg)

#include <draw/path.h>

namespace draw
{

draw::path iconCheckBoxEmpty( void )
{
	draw::path path;

	// SVG: M19 5v14H5V5h14m0-2H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h14c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2z

	path.move_to( { 19.0, 5.0 } );

	path.vertical_by( 14.0 );

	path.horizontal_to( 5.0 );

	path.vertical_to( 5.0 );

	path.horizontal_by( 14.0 );

	path.close();
	path.move_by( { 0.0, -2.0 } );

	path.horizontal_to( 5.0 );

	path.cubic_by( { -1.1, 0.0 }, { -2.0, 0.9 }, { -2.0, 2.0 } );

	path.vertical_by( 14.0 );

	path.cubic_by( { 0.0, 1.1 }, { 0.9, 2.0 }, { 2.0, 2.0 } );

	path.horizontal_by( 14.0 );

	path.cubic_by( { 1.1, 0.0 }, { 2.0, -0.9 }, { 2.0, -2.0 } );

	path.vertical_to( 5.0 );

	path.cubic_by( { 0.0, -1.1 }, { -0.9, -2.0 }, { -2.0, -2.0 } );

	path.close();

	return path;
}

}
