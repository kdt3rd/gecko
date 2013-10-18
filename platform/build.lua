
--SubDir( "sdl" )
SubDir( "xcb" )

srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"dispatcher.cpp";
	"painter.cpp";
}

Library( "platform", Compile( srcs ) )

