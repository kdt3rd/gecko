
Include( XCB_INCLUDE )

srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"keyboard.cpp";
	"mouse.cpp";
	"dispatcher.cpp";
	"painter.cpp";
}

Library( "platform-xcb", Compile( srcs ), LinkSys( XCB_LIBS ) )

