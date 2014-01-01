
Include( XLIB_INCLUDE, CAIRO_INCLUDE, FONTCONFIG_INCLUDE )

srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"keyboard.cpp";
	"mouse.cpp";
	"font_manager.cpp";
	"dispatcher.cpp";
}

Library( "platform-xlib", Compile( srcs ), LinkLibs( "platform", "draw-cairo" ), LinkSys( XLIB_LIBS, CAIRO_LIBS, FREETYPE_LIBS, FONTCONFIG_LIBS ) )

