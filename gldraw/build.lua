
srcs = {
	"drawable.cpp";
	"object.cpp";
	"path.cpp";
	"geometry.cpp";
	"clipper.cpp";
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

Library( "gldraw", Compile( srcs ), Compile( tess ), LinkLibs( "gl" ) );

