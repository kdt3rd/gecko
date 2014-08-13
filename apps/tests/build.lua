
--Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", platform ) )
Executable( "test_format", Compile( "test_format.cpp" ), LinkLibs( "base", "utf" ) )
Executable( "test_exception", Compile( "test_exception.cpp" ), LinkLibs( "base", "utf" ) )
Executable( "test_signal", Compile( "test_signal.cpp" ), LinkLibs( "base" ) )
Executable( "test_platform", Compile( "test_platform.cpp" ), LinkLibs( "platform" ) )
Executable( "test_app", Compile( "test_app.cpp" ), LinkLibs( "gui" ) )
Executable( "test_font", Compile( "test_font.cpp" ), LinkLibs( "script" ) )
Executable( "test_hash", Compile( "test_hash.cpp" ), LinkLibs( "base" ) )
Executable( "test_mempool", Compile( "test_mempool.cpp" ), LinkLibs( "base" ) )

Application( "Test Application", "test_app" )
Application( "Test Platform", "test_platform" )
--Application( "Test", "test" )

