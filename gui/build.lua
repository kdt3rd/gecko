
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

Library( "gui", Compile( srcs ), DataCompile( "shaders", table.unpack( shaders ) ), LinkLibs( "platform", "layout", "utf" ) );

