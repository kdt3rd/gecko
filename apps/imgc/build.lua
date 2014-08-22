
Include( PNG_INCLUDE );

Executable( "imgc", Compile( "main.cpp" ), LinkLibs( "imgproc" ) )
Executable( "test_buf", Compile( "test_buf.cpp", "histogram.cpp", "png_reader.cpp" ), LinkLibs( "imgproc" ), LinkSys( PNG_LIBS ) )

