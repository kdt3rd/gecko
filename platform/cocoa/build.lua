
Platform {
	name = "cocoa";

	incs = {
		COCOA_INCLUDE;
	};

	srcs = {
		"system.mm";
		"screen.cpp";
		"window.mm";
		"timer.cpp";
		"keyboard.mm";
		"mouse.cpp";
		"font_manager.mm";
		"dispatcher.mm";
	};

	libs = {
	};

	syslibs = {
		COCOA_LIBS;
	};
}

