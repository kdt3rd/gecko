
Include( CAIRO_INCLUDE, FREETYPE_INCLUDE, FONTCONFIG_INCLUDE )

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

Library( "platform-mswin", Compile( srcs ), LinkLibs( "platform", "draw-cairo" ), LinkSys( CAIRO_LIBS, FONTCONFIG_LIBS, FREETYPE_LIBS ) )

