
Include( source_dir )

srcs = {
	"format.cpp";
	"contract.cpp";
	"rect.cpp";
	"color.cpp";
	"path.cpp";
	"string_util.cpp";
	"gradient.cpp";
	"paint.cpp";
	"pack.cpp";
	"sha256.cpp";
	"backtrace.cpp";
}

Library( "base", Compile( srcs ) );

