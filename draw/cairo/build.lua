
Include( CAIRO_INCLUDE )

srcs = {
	"canvas.cpp";
	"font.cpp";
}

Library( "draw-cairo", Compile( srcs ), LinkLibs( { "core", "draw" } ), LinkSys( CAIRO_LIBS, FONTCONFIG_LIBS, FREETYPE_LIBS ) )

