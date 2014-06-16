
Include( source_dir )
Include( BuildFile() )

SubDir( "core" )
SubDir( "gl" )
SubDir( "utf" )
SubDir( "layout" )
SubDir( "draw" )
SubDir( "platform" )
SubDir( "gui" )

--Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( "platform", "draw", "utf" ) )
Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui" ) )

Application( "Test Application", "test_app" )
--Application( "Test", "test" )

Doxygen( "docs", "doxyfile", "", "docs" );

