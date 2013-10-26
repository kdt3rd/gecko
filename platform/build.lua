
srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"keyboard.cpp";
	"mouse.cpp";
	"dispatcher.cpp";
}

Library( "platform", Compile( srcs ), LinkLibs( "draw" ) )

--SubDir( "sdl" )
SubDir( "xcb" )

