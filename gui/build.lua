
srcs = {
	"application.cpp";
	"layouts.cpp";
	"window.cpp";
	"style.cpp";
	"cocoa_style.cpp";
	"label.cpp";
	"button.cpp";
	"slider.cpp";
}

Library( "gui", Compile( srcs ), LinkLibs( "layout", "reaction" ) );

