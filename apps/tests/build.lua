
--Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ) )
Executable( "test_signal", Compile( "test_signal.cpp" ), LinkLibs( "base" ) )

--Executable( "test", Compile( "test.cpp" ), LinkLibs( "platform", "draw", "gl", "utf", "gui" ), LinkSys( GL_LIBS ) )
--Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui" ) )

--Application( "Test Application", "test_app" )
--Application( "Test", "test" )

