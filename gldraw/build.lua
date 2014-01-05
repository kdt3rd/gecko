
srcs = {
	"path.cpp";
	"geometry.cpp";
}

Library( "gldraw", Compile( srcs ), LinkLibs( "core" ) );

