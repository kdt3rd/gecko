
Include( source_dir, ALLEG_INCLUDE, IMG_INCLUDE, MAIN_INCLUDE, PRIM_INCLUDE )

SubDir( "allegro" )
SubDir( "core" )
SubDir( "gui" )

Executable( "test", Compile( "test.cpp" ), LinkLibs( "allegropp", "gui" ), LinkSys( ALLEG_LIBS, IMG_LIBS, MAIN_LIBS, PRIM_LIBS ) )
