
Include( XCB_INCLUDE, CAIRO_INCLUDE, FONTCONFIG_INCLUDE )

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

Library( "platform-dummy", Compile( srcs ), LinkLibs( "platform", "draw-dummy" ) )

