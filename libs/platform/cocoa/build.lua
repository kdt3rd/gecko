
Platform {
	name = "cocoa";

	incs = {
		COCOA_INCLUDE;
	};

	srcs = {
		"system.mm";
		"screen.mm";
		"window.mm";
		"timer.cpp";
		"keyboard.mm";
		"mouse.cpp";
		"dispatcher.mm";
	};

	libs = {
	};

	syslibs = {
		COCOA_LIBS;
	};
}

