
Platform {
	name = "fc";

	incs = {
		FREETYPE_INCLUDE;
		FONTCONFIG_INCLUDE;
	};

	srcs = {
		"font_manager.cpp";
	};

	libs = {
	};

	syslibs = {
		FREETYPE_LIBS;
		FONTCONFIG_LIBS;
	};
}

