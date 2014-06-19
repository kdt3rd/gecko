
Platform {
	name = "mswin";

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
		"gl";
	};

	syslibs = {
		GL_LIBS;
	};
}

