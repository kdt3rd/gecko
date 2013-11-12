
srcs = {
	"application.cpp";
	"container.cpp";
	"window.cpp";
	"builder.cpp";
	"style.cpp";
	"cocoa_style.cpp";
}

Library( "gui", Compile( srcs ), LinkLibs( "view", "layout", "reaction" ) );

