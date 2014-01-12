
Include( GL_INCLUDE )

srcs = {
	"texture.cpp";
	"shader.cpp";
	"program.cpp";
	"context.cpp";
	"matrix4.cpp";
}

Library( "gl", Compile( srcs ), LinkLibs( "core" ), LinkSys( GL_LIBS ) );
