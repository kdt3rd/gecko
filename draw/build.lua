
SubDir( "cairo" )

srcs = {
	"color.cpp";
	"canvas.cpp";
	"paint.cpp";
	"path.cpp";
	"font.cpp";
}

Library( "draw", Compile( srcs ) );
