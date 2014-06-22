
Include( SourceFile() )
Include( SourceFile( "libs" ) )
Include( BuildFile() )
Include( BuildFile( "libs" ) )
Include( GL_INCLUDE )

SubDir( "libs" )
SubDir( "apps" )

Doxygen( "docs", SourceFile( "docs/doxyfile" ), "", "docs" );
