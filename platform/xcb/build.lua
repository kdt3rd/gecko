
Include( XCB_INCLUDE )

srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"dispatcher.cpp";
	"painter.cpp";
}

Library( "platform-xcb", Compile( srcs ), LinkSys( XCB_LIBS ) )

