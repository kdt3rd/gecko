
Include( source_dir, BOTAN_INCLUDE, FREETYPE_INCLUDE, CAIRO_INCLUDE )

SubDir( "core" )
SubDir( "draw" )
SubDir( "store" )
SubDir( "layout" )
SubDir( "platform" )

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "layout", "platform-xcb" ), LinkSys( BOTAN_LIBS ) )
Executable( "test", Compile( "test.cpp" ), LinkLibs( "platform-xcb" ), LinkSys( BOTAN_LIBS ) )
Doxygen( "docs", "doxyfile", "", "docs" );
