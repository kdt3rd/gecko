
srcs = {
	"application.cpp";
	"layouts.cpp";
	"context.cpp";
	"window.cpp";
	"style.cpp";
	"cocoa_style.cpp";
	"color.cpp";
	"background.cpp";
	"label.cpp";
	"button.cpp";
	"slider.cpp";
	"line_edit.cpp";
	"scroll_area.cpp";
	"container.cpp";
	"tree_node.cpp";
}

Library( "gui", Compile( srcs ), LinkLibs( "platform", "layout", "utf" ) );

