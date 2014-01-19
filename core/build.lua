
Include( source_dir )

srcs = {
	"contract.cpp";
	"rect.cpp";
	"color.cpp";
	"path.cpp";
	"string.cpp";
}

Library( "core", Compile( srcs ) );

