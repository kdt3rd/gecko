
--Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ) )
Executable( "test_signal", Compile( "test_signal.cpp" ), LinkLibs( "base" ) )
Executable( "test_platform", Compile( "test_platform.cpp" ), LinkLibs( "platform" ) )
Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui" ) )
Executable( "test_font", Compile( "test_font.cpp" ), LinkLibs( "script" ) )

--Application( "Test Application", "test_app" )
--Application( "Test", "test" )

