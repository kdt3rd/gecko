
Include( source_dir )

SubDir( "core" )
SubDir( "gl" )
SubDir( "utf" )
SubDir( "platform" )
SubDir( "layout" )
SubDir( "gldraw" )
SubDir( "gui" )

--Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ) )
--Executable( "test", Compile( "test.cpp" ), LinkLibs( platform ) )
Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui" ) )

Application( "Test Application", "test_app" )
--Application( "Test", "test" )

Doxygen( "docs", "doxyfile", "", "docs" );

