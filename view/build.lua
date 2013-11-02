
srcs = {
	"view.cpp";
	"text.cpp";
}

cocoa = {
	"cocoa/button.cpp";
}

Library( "view", Compile( srcs, cocoa ), LinkLibs( "draw" ) );

