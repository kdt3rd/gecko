
Include( source_dir )

srcs = {
	"font.cpp";
	"font_manager.cpp";
}

syslibs = nil

if System() == "Darwin" then
	table.insert( srcs, "cocoa/font_manager.mm" )
elseif System() == "Linux" then
	Include( FREETYPE_INCLUDE, FONTCONFIG_INCLUDE )
	table.insert( srcs, "fontconfig/font_manager.cpp" )
	table.insert( srcs, "fontconfig/font.cpp" )
	syslibs = LinkSys( FREETYPE_LIBS, FONTCONFIG_LIBS )
	print( syslibs )
end

Library( "script", Compile( srcs ), LinkLibs( "base" ), syslibs )

