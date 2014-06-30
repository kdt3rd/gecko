
Include( GL_INCLUDE )

srcs = {
	"texture.cpp";
	"shader.cpp";
	"program.cpp";
	"context.cpp";
	"matrix4.cpp";
	"check.cpp";
	"gl3w.c";
}

Library( "gl", Compile( srcs ), LinkLibs( "base" ), LinkSys( GL_LIBS ) );
