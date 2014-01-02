
Platform {
	name = "xcb";

	incs = {
		CAIRO_INCLUDE;
	};

	srcs = {
		"system.cpp";
		"screen.cpp";
		"window.cpp";
		"timer.cpp";
		"keyboard.cpp";
		"mouse.cpp";
		"dispatcher.cpp";
		"atom.cpp";
	};

	libs = {
		"draw-cairo";
	};

	syslibs = {
		XCB_LIBS;
	};
}

