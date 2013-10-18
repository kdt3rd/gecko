
Include( source_dir )

srcs = {
	"contract.cpp";
	"area.cpp";
	"container.cpp";
}

Library( "core", Compile( srcs ) );

