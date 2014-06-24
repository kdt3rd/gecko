
Platform {
	name = "xlib";

	incs = {
		GL_INCLUDE;
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
		"draw";
		"utf";
	};

	syslibs = {
		XLIB_LIBS;
		GL_LIBS;
	};
}

