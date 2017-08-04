// Automatically generated from SVG
// (from file /home/iangodin/Sources/material-design-icons/toggle/svg/design/ic_radio_button_checked_24px.svg)

#include <draw/path.h>

namespace draw
{

draw::path iconRadioButtonChecked( void )
{
	draw::path path;

	// SVG: M12 7c-2.76 0-5 2.24-5 5s2.24 5 5 5 5-2.24 5-5-2.24-5-5-5zm0-5C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8z

	path.move_to( { 12.0, 7.0 } );

	path.cubic_by( { -2.76, 0.0 }, { -5.0, 2.24 }, { -5.0, 5.0 } );

	path.cubic_by( { 2.24, 5.0 }, { 5.0, 5.0 } );
	path.cubic_by( { 5.0, -2.24 }, { 5.0, -5.0 } );
	path.cubic_by( { -2.24, -5.0 }, { -5.0, -5.0 } );

	path.close();

	path.move_by( { 0.0, -5.0 } );

	path.cubic_to( { 6.48, 2.0 }, { 2.0, 6.48 }, { 2.0, 12.0 } );

	path.cubic_by( { 4.48, 10.0 }, { 10.0, 10.0 } );
	path.cubic_by( { 10.0, -4.48 }, { 10.0, -10.0 } );

	path.cubic_to( { 17.52, 2.0 }, { 12.0, 2.0 } );

	path.close();

	path.move_by( { 0.0, 18.0 } );

	path.cubic_by( { -4.42, 0.0 }, { -8.0, -3.58 }, { -8.0, -8.0 } );

	path.cubic_by( { 3.58, -8.0 }, { 8.0, -8.0 } );
	path.cubic_by( { 8.0, 3.58 }, { 8.0, 8.0 } );
	path.cubic_by( { -3.58, 8.0 }, { -8.0, 8.0 } );

	path.close();

	return path;
}

}

