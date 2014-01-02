
Platform {
	name = "xlib";

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
	};

	libs = {
		"draw-cairo";
	};

	syslibs = {
		XLIB_LIBS;
	};
}

