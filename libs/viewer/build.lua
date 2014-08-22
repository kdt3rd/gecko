
Include( BuildFile() )

srcs = {
	"viewer.cpp";
}

shaders = {
	"shader.vert";
	"shader.frag";
	"errorA.frag";
	"errorB.frag";
	"textureA.frag";
	"textureB.frag";
	"nolutA.frag";
	"nolutB.frag";
	"nowipe.frag";
}

local cpp_shaders = {
	"#include \"shaders.h\"";
	"using base::resource;";
	"namespace viewer";
	"{";
	"	base::data_resource shaders( {";
}
for i, file in pairs( shaders ) do
	local data = io.open( SourceFile( file ), "r" )
	data = data:read( "*a" )
	table.insert( cpp_shaders, string.format( "\tresource { \"%s\",", file ) )
	table.insert( cpp_shaders, "\t\t{" )
	BinaryToCString( data, cpp_shaders, "\t\t\t" )
	table.insert( cpp_shaders, string.format( "\t\t}, %d", #data ) )
	table.insert( cpp_shaders, "\t}," )
end
table.insert( cpp_shaders, "\t} );" )
table.insert( cpp_shaders, "}" )

table.insert( srcs, CreateFile( "shaders.cpp", cpp_shaders, shaders ) )

CreateFile( "shaders.h", {
	"#pragma once";
	"#include <base/data_resource.h>";
	"namespace viewer";
	"{";
	"extern base::data_resource shaders;";
	"}";
} )

Library( "viewer", Compile( srcs ), LinkLibs( "gl" ) );

