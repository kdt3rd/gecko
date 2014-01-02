
Platform {
	name = "cocoa";

	incs = {
		COCOA_INCLUDE;
		CAIRO_INCLUDE;
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
		"draw-cairo";
	};

	syslibs = {
		COCOA_LIBS;
		CAIRO_LIBS;
	};
}

