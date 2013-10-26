
Include( source_dir, BOTAN_INCLUDE, FREETYPE_INCLUDE, CAIRO_INCLUDE )

SubDir( "core" )
SubDir( "draw" )
SubDir( "store" )
SubDir( "layout" )
SubDir( "platform" )

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "core", "layout", "platform-xcb", "platform", "draw-cairo", "draw" ), LinkSys( SDL_LIBS, BOTAN_LIBS, XCB_LIBS, CAIRO_LIBS, FREETYPE_LIBS ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( "core", "platform-xcb", "platform", "draw-cairo", "draw" ), LinkSys( SDL_LIBS, BOTAN_LIBS, XCB_LIBS, CAIRO_LIBS, FREETYPE_LIBS ) )
Doxygen( "docs", "doxyfile", "", "docs" );
