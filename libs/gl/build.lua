
Include( GL_INCLUDE )
Include( PNG_INCLUDE )

srcs = {
	"texture.cpp";
	"shader.cpp";
	"program.cpp";
	"context.cpp";
	"matrix4.cpp";
	"check.cpp";
	"png_reader.cpp";
	"gl3w.c";
}

Library( "gl", Compile( srcs ), LinkLibs( "base" ), LinkSys( GL_LIBS, PNG_LIBS ) );
