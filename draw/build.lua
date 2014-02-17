
srcs = {
	"drawable.cpp";
	"polylines.cpp";
	"clipper.cpp";
	"canvas.cpp";
	"font.cpp";
	"geometry.cpp";
	"glyph.cpp";
	"composite.cpp";
	"object.cpp";
	"stretchable.cpp";
}

tess = {
	"libtess2/bucketalloc.c";
	"libtess2/dict.c";
	"libtess2/geom.c";
	"libtess2/mesh.c";
	"libtess2/priorityq.c";
	"libtess2/sweep.c";
	"libtess2/tess.c";
}

shaders = {
	"color_mesh.vert";
	"position_uv.vert";
	"quadrant.vert";
	"simple.vert";
	"text.vert";
	"color_mesh.frag";
	"linear_gradient.frag";
	"single_color.frag";
	"text_bitmap.frag";
}

local cpp_shaders = {
	"#include \"shaders.h\"";
	"using core::resource;";
	"namespace draw";
	"{";
	"	core::data_resource shaders( {";
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
	"#include <core/data_resource.h>";
	"namespace draw";
	"{";
	"extern core::data_resource shaders;";
	"}";
} )

Library( "draw", Compile( srcs ), Compile( tess ), LinkLibs( "gl" ) );

