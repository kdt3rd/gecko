
SubDir( "cairo" )

srcs = {
	"color.cpp";
	"canvas.cpp";
	"paint.cpp";
	"path.cpp";
}

Library( "draw", Compile( srcs ) );
