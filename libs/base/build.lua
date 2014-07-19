
Include( source_dir )

srcs = {
	"contract.cpp";
	"rect.cpp";
	"color.cpp";
	"path.cpp";
	"string_util.cpp";
	"gradient.cpp";
	"paint.cpp";
	"pack.cpp";
	"sha256.cpp";
}

Library( "base", Compile( srcs ) );

