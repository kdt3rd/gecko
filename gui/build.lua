
srcs = {
	"application.cpp";
	"layouts.cpp";
	"label.cpp";
	"window.cpp";
	"style.cpp";
	"cocoa_style.cpp";
}

Library( "gui", Compile( srcs ), LinkLibs( "layout", "reaction" ) );

