
Platform {
	name = "fc";

	srcs = {
		"font_manager.cpp";
	};

	libs = {
		"draw-cairo";
	};

	syslibs = {
		FREETYPE_LIBS;
		FONTCONFIG_LIBS;
	};
}

