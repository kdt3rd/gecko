
Include( GL_INCLUDE )

srcs = {
	"texture.cpp";
	"shader.cpp";
	"program.cpp";
	"context.cpp";
}

Library( "gl", Compile( srcs ), LinkSys( GL_LIBS ) );
