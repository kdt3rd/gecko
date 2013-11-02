
srcs = {
	"view.cpp";
	"text.cpp";
	"flat.cpp";
}

cocoa = {
	"cocoa/button.cpp";
}

Library( "view", Compile( srcs, cocoa ), LinkLibs( "draw" ) );

