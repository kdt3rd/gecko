
--Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ) )
Executable( "test_signal", Compile( "test_signal.cpp" ), LinkLibs( "base" ) )
Executable( "test_platform", Compile( "test_platform.cpp" ), LinkLibs( "platform" ) )
Executable( "test_script", Compile( "test_script.cpp" ), LinkLibs( "script" ), LinkSys( "-framework AppKit" ) )
Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui" ) )

Application( "Test Application", "test_app" )
Application( "Test Platform", "test_platform" )
--Application( "Test", "test" )

