// Automatically generated from SVG
// (from file /home/iangodin/Sources/material-design-icons/toggle/svg/design/ic_check_box_24px.svg)

#include <draw/icons.h>

namespace draw
{

draw::path iconCheckBoxChecked( void )
{
	draw::path path;

	// SVG: M19 3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.11 0 2-.9 2-2V5c0-1.1-.89-2-2-2zm-9 14l-5-5 1.41-1.41L10 14.17l7.59-7.59L19 8l-9 9z

	path.move_to( { 19.0, 3.0 } );

	path.horizontal_to( 5.0 );

	path.cubic_by( { -1.11, 0.0 }, { -2.0, 0.9 }, { -2.0, 2.0 } );

	path.vertical_by( 14.0 );

	path.cubic_by( { 0.0, 1.1 }, { 0.89, 2.0 }, { 2.0, 2.0 } );

	path.horizontal_by( 14.0 );

	path.cubic_by( { 1.11, 0.0 }, { 2.0, -0.9 }, { 2.0, -2.0 } );

	path.vertical_to( 5.0 );

	path.cubic_by( { 0.0, -1.1 }, { -0.89, -2.0 }, { -2.0, -2.0 } );

	path.close();

	path.move_by( { -9.0, 14.0 } );

	path.line_by( { -5.0, -5.0 } );
	path.line_by( { 1.41, -1.41 } );

	path.line_to( { 10.0, 14.17 } );

	path.line_by( { 7.59, -7.59 } );

	path.line_to( { 19.0, 8.0 } );

	path.line_by( { -9.0, 9.0 } );

	path.close();

	return path;
}

}
