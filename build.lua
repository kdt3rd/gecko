
Include( source_dir, BOTAN_INCLUDE, FREETYPE_INCLUDE, CAIRO_INCLUDE )

SubDir( "core" )
SubDir( "draw" )
SubDir( "store" )
SubDir( "layout" )
SubDir( "platform" )

if System() == "Linux" then
	platform = "platform-xcb"
	CXXFlags( "-DPLATFORM=xcb" )
elseif System() == "Darwin" then
	platform = { "platform-cocoa", "platform-xcb" }
	CXXFlags( "-DPLATFORM=cocoa" )
else
	error( "unknown platform" )
end

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ), LinkSys( BOTAN_LIBS ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( platform ), LinkSys( BOTAN_LIBS ) )

Doxygen( "docs", "doxyfile", "", "docs" );

