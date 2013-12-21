
srcs = {
	"application.cpp";
	"layouts.cpp";
	"window.cpp";
	"style.cpp";
	"cocoa_style.cpp";
	"label.cpp";
	"button.cpp";
	"slider.cpp";
	"scroll_area.cpp";
}

Library( "gui", Compile( srcs ), LinkLibs( "layout", "reaction" ) );

