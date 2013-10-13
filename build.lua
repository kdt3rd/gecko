
Include( source_dir, ALLEG_INCLUDE, IMG_INCLUDE, MAIN_INCLUDE, PRIM_INCLUDE, FONT_INCLUDE, BOTAN_INCLUDE )

SubDir( "allegro" )
SubDir( "core" )
SubDir( "store" )
SubDir( "gui" )

Executable( "test_layout", Compile( "test_layout.cpp" ), LinkLibs( "core", "allegropp", "gui" ), LinkSys( ALLEG_LIBS, IMG_LIBS, MAIN_LIBS, PRIM_LIBS, FONT_LIBS, BOTAN_LIBS ) )
