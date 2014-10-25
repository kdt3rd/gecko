
Include( PNG_INCLUDE );


local imgc = {
	SourceFile( "integrate.imgc" );
	SourceFile( "filters.imgc" );
}

local imgcpp = BuildFile( "test.cpp" )

--Run( imgc, imgcpp, BuildFile(), BinFile( "imgc" ), imgc, imgcpp )

Executable( "imgc", Compile( "main.cpp" ), LinkLibs( "imgproc" ) )
--Executable( "test_buf", Compile( "test_buf.cpp", "png_reader.cpp", imgcpp ), LinkLibs( "imgproc" ), LinkSys( PNG_LIBS ) )
Executable( "test_new", Compile( "newtest.cpp" ), LinkLibs( "imgproc" ) )

