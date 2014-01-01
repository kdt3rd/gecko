
Include( COCOA_INCLUDE, CAIRO_INCLUDE )

srcs = {
	"system.mm";
	"screen.cpp";
	"window.mm";
	"timer.cpp";
	"keyboard.mm";
	"mouse.cpp";
	"font_manager.mm";
	"dispatcher.mm";
}

Library( "platform-cocoa", Compile( srcs ), LinkLibs( "platform", "draw-cairo" ), LinkSys( COCOA_LIBS, CAIRO_LIBS ) )

