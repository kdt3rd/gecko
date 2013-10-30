
srcs = {
	"color.cpp";
	"rect.cpp";
	"area.cpp";
	"canvas.cpp";
	"paint.cpp";
	"path.cpp";
	"font.cpp";
}

Library( "draw", Compile( srcs ), LinkLibs( "core" ) );

SubDir( "cairo" )
