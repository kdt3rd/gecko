
Include( SourceFile() )
SubDir( "tables" );

local srcs = {
	"utf.cpp";
}

local gens = {
	BuildFile( "utfprop.cpp" );
	BuildFile( "utfcat.cpp" );
	BuildFile( "utfnorm.cpp" );
}

local xml = SourceFile( "tables/ucd.all.flat.xml" );

Run( xml, gens, BuildFile(), BinFile( "gen_table" ), xml )

Library( "utf", Compile( srcs ), Compile( gens ) );

Executable( "utf_test", Compile( "test.cpp" ), LinkLibs( "utf" ) )

