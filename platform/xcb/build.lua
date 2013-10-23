
Include( XCB_INCLUDE, CAIRO_INCLUDE )

srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"keyboard.cpp";
	"mouse.cpp";
	"dispatcher.cpp";
}

Library( "platform-xcb", Compile( srcs ), LinkSys( XCB_LIBS, CAIRO_LIBS ) )

