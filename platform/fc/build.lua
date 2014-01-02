
Include( CAIRO_INCLUDE, FREETYPE_INCLUDE, FONTCONFIG_INCLUDE )

srcs = {
	"font_manager.cpp";
}

Library( "platform-fc", Compile( srcs ), LinkLibs( "platform", "draw-cairo" ), LinkSys( CAIRO_LIBS, FREETYPE_LIBS, FONTCONFIG_LIBS ) )

