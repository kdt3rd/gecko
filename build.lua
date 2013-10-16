
Include( source_dir, SDL_INCLUDE, BOTAN_INCLUDE )

SubDir( "core" )
SubDir( "store" )
SubDir( "layout" )
SubDir( "platform" )

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "core", "layout", "platform-sdl", "platform" ), LinkSys( SDL_LIBS, BOTAN_LIBS ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( "core", "platform-sdl", "platform" ), LinkSys( SDL_LIBS, BOTAN_LIBS ) )
