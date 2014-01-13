
Include( source_dir )

srcs = {
	"contract.cpp";
	"rect.cpp";
	"color.cpp";
	"path.cpp";
}

Library( "core", Compile( srcs ) );

