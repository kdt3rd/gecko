
Include( COCOA_INCLUDE, CAIRO_INCLUDE )

srcs = {
	"system.mm";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"keyboard.cpp";
	"mouse.cpp";
	"font_manager.cpp";
	"dispatcher.mm";
}

Library( "platform-cocoa", Compile( srcs ), LinkLibs( "platform", "draw-cairo" ), LinkSys( COCOA_LIBS, CAIRO_LIBS ) )

