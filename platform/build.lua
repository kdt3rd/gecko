
SubDir( "sdl" )

srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"painter.cpp";
}

Library( "platform", Compile( srcs ) )

