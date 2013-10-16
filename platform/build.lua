
SubDir( "sdl" )

srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"dispatcher.cpp";
	"painter.cpp";
}

Library( "platform", Compile( srcs ) )

