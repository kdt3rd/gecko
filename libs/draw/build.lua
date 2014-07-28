
Include( BuildFile() )

srcs = {
	"drawable.cpp";
	"polylines.cpp";
	"clipper.cpp";
	"canvas.cpp";
	"geometry.cpp";
	"glyph.cpp";
	"composite.cpp";
	"object.cpp";
	"stretchable.cpp";
	"color_wheel.cpp";
}

tess = {
	"libtess2/bucketalloc.cpp";
	"libtess2/dict.cpp";
	"libtess2/geom.cpp";
	"libtess2/mesh.cpp";
	"libtess2/priorityq.cpp";
	"libtess2/sweep.cpp";
	"libtess2/tess.cpp";
}

shaders = {
	"color_mesh.vert";
	"position_uv.vert";
	"quadrant.vert";
	"simple.vert";
	"text.vert";
	"texture.frag";
	"color_mesh.frag";
	"linear_gradient.frag";
	"radial_gradient.frag";
	"conical_gradient.frag";
	"single_color.frag";
	"text_bitmap.frag";
}

local cpp_shaders = {
	"#include \"shaders.h\"";
	"using base::resource;";
	"namespace draw";
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
	"namespace draw";
	"{";
	"extern base::data_resource shaders;";
	"}";
} )

Library( "draw", Compile( srcs ), Compile( tess ), LinkLibs( "gl", "script" ) );

