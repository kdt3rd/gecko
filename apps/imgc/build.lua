
Include( PNG_INCLUDE );


local histo = SourceFile( "histogram.imgc" )
local hcpp = BuildFile( "histogram.cpp" )
Run( histo, hcpp, BuildFile(), BinFile( "imgc" ), histo, hcpp )

Executable( "imgc", Compile( "main.cpp" ), LinkLibs( "imgproc" ) )
Executable( "test_buf", Compile( "test_buf.cpp", "png_reader.cpp", hcpp ), LinkLibs( "imgproc" ), LinkSys( PNG_LIBS ) )

