
Include( BuildFile() )

srcs = {
	"application.cpp";
	"layouts.cpp";
	"context.cpp";
	"window.cpp";
	"popup.cpp";
	"background.cpp";
	"background_color.cpp";
	"label.cpp";
	"button.cpp";
	"slider.cpp";
	"line_edit.cpp";
--	"scroll_area.cpp";
	"container.cpp";
--	"tree_node.cpp";
	"color_picker.cpp";
}

Library( "gui", Compile( srcs ), LinkLibs( "platform", "layout", "utf" ) );

