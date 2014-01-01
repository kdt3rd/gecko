
Include( source_dir, CAIRO_INCLUDE )

if System() == "Linux" then
	platform = "platform-xlib"
	CXXFlags( "-DPLATFORM=xlib" )
elseif System() == "Darwin" then
	platform = "platform-cocoa"
	CXXFlags( "-DPLATFORM=cocoa" )
elseif System() == "Windows" then
	platform = "platform-mswin"
	CXXFlags( "-DPLATFORM=mswin" )
else
	error( "unknown platform" )
end

SubDir( "core" )
SubDir( "utf" )
SubDir( "draw" )
SubDir( "platform" )
SubDir( "layout" )
SubDir( "gui" )

--Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ) )
--Executable( "test", Compile( "test.cpp" ), LinkLibs( platform ) )
Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui", platform ) )

Application( "Test Application", "test_app" )
--Application( "Test", "test" )

Doxygen( "docs", "doxyfile", "", "docs" );

