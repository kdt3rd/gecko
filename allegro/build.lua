
Include( source_dir, ALLEG_INCLUDE, MAIN_INCLUDE, IMG_INCLUDE )

Library( "allegropp", Compile( "system.cpp" ), LinkSys( ALLEG_LIBS, IMG_LIBS ) )

