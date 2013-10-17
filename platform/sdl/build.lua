
srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"dispatcher.cpp";
	"painter.cpp";
}

Library( "platform-sdl", Compile( srcs ) )

