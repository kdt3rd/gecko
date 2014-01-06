
srcs = {
	"path.cpp";
	"geometry.cpp";
	"clipper.cpp";
}

Library( "gldraw", Compile( srcs ), LinkLibs( "core" ) );

