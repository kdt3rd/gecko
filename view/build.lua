
srcs = {
	"view.cpp";
}

Library( "view", Compile( srcs ), LinkLibs( "draw" ) );

SubDir( "cocoa" );
