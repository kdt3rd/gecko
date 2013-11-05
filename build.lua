
Include( source_dir, BOTAN_INCLUDE, FREETYPE_INCLUDE, CAIRO_INCLUDE )

if System() == "Linux" then
	platform = "platform-xcb"
	CXXFlags( "-DPLATFORM=xcb" )
elseif System() == "Darwin" then
	platform = "platform-cocoa"
	CXXFlags( "-DPLATFORM=cocoa" )
elseif System() == "Windows" then
	platform = "platform-dummy"
	CXXFlags( "-DPLATFORM=dummy" )
else
	error( "unknown platform" )
end
print( platform )

SubDir( "core" )
SubDir( "draw" )
SubDir( "view" )
SubDir( "layout" )
SubDir( "platform" )
SubDir( "react" )
SubDir( "gui" )

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ), LinkSys( BOTAN_LIBS ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( "view", platform ), LinkSys( BOTAN_LIBS ) )
Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui", platform ) )

Application( "Test Application", "test_app" )

Doxygen( "docs", "doxyfile", "", "docs" );

