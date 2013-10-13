
Include( source_dir, ALLEG_INCLUDE, IMG_INCLUDE, MAIN_INCLUDE, PRIM_INCLUDE, FONT_INCLUDE, BOTAN_INCLUDE )

SubDir( "allegro" )
SubDir( "core" )
SubDir( "store" )
SubDir( "layout" )
SubDir( "platform" )

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "core", "allegropp", "layout" ), LinkSys( ALLEG_LIBS, IMG_LIBS, MAIN_LIBS, PRIM_LIBS, FONT_LIBS, BOTAN_LIBS ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( "core", "platform-sdl", "platform" ), LinkSys( SDL_LIBS ) )
