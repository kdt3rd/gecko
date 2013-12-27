
srcs = {
	"application.cpp";
	"layouts.cpp";
	"window.cpp";
	"style.cpp";
	"cocoa_style.cpp";
	"color.cpp";
	"label.cpp";
	"button.cpp";
	"slider.cpp";
	"scroll_area.cpp";
	"container.cpp";
	"tree_node.cpp";
}

Library( "gui", Compile( srcs ), LinkLibs( "layout" ) );

