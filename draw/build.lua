
SubDir( "cairo" )

srcs = {
	"canvas.cpp";
	"paint.cpp";
	"path.cpp";
}

Library( "draw", Compile( srcs ) );
