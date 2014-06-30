
Include( source_dir )

srcs = {
	"glyph.cpp";
	"font.cpp";
	"font_manager.cpp";
	"cocoa/font_manager.mm";
}

Library( "script", Compile( srcs ), LinkLibs( "base" ) );

