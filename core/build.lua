
Include( source_dir )

srcs = {
	"contract.cpp";
	"rect.cpp";
	"color.cpp";
}

Library( "core", Compile( srcs ) );

