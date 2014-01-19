
srcs = {
	"drawable.cpp";
	"object.cpp";
	"polylines.cpp";
	"clipper.cpp";
	"canvas.cpp";
	"paint.cpp";
	"geometry.cpp";
	"composite.cpp";
}

tess = {
	"libtess2/bucketalloc.c";
	"libtess2/dict.c";
	"libtess2/geom.c";
	"libtess2/mesh.c";
	"libtess2/priorityq.c";
	"libtess2/sweep.c";
	"libtess2/tess.c";
}

Library( "draw", Compile( srcs ), Compile( tess ), LinkLibs( "gl" ) );

