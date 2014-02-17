
Include( source_dir )

srcs = {
	"contract.cpp";
	"rect.cpp";
	"color.cpp";
	"path.cpp";
	"string.cpp";
	"gradient.cpp";
	"paint.cpp";
	"pack.cpp";
}

Library( "core", Compile( srcs ) );

