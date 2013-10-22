
Include( CAIRO_INCLUDE )

srcs = {
	"canvas.cpp";
}

Library( "draw-cairo", Compile( srcs ) )

