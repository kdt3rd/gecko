
srcs = {
	"canvas.cpp";
	"font.cpp";
}

Library( "draw-dummy", Compile( srcs ), LinkLibs( { "core", "draw" } ) )

