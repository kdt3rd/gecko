
Include( source_dir, SDL_INCLUDE, BOTAN_INCLUDE )

SubDir( "core" )
SubDir( "store" )
SubDir( "layout" )
SubDir( "platform" )

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "core", "layout", "platform-xcb", "platform" ), LinkSys( SDL_LIBS, BOTAN_LIBS, XCB_LIBS ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( "core", "platform-xcb", "platform" ), LinkSys( SDL_LIBS, BOTAN_LIBS, XCB_LIBS ) )
