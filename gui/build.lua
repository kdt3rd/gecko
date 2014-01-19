
Include( BuildFile() )

srcs = {
	"application.cpp";
	"layouts.cpp";
	"context.cpp";
	"window.cpp";
	"style.cpp";
	"cocoa_style.cpp";
	"background.cpp";
	"background_color.cpp";
	"label.cpp";
	"button.cpp";
	"slider.cpp";
	"line_edit.cpp";
	"scroll_area.cpp";
	"container.cpp";
	"tree_node.cpp";
	"color_picker.cpp";
}

shaders = {
	"color_mesh.vert";
	"position_uv.vert";
	"quadrant.vert";
	"simple.vert";
	"color_mesh.frag";
	"linear_gradient.frag";
	"single_color.frag";
}

local cpp_shaders = {
	"#include \"shaders.h\"";
	"using core::resource;";
	"namespace gui";
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
table.insert( srcs, CreateFile( "shaders.cpp", cpp_shaders ) )

local h_shaders = {
	"#pragma once";
	"#include <core/data_resource.h>";
	"namespace gui";
	"{";
	"extern core::data_resource shaders;";
	"}";
}
CreateFile( "shaders.h", h_shaders )

Library( "gui", Compile( srcs ), LinkLibs( "platform", "layout", "utf" ) );

