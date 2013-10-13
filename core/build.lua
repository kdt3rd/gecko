
Include( source_dir )

srcs = {
	"contract.cpp";
	"rectangle.cpp";
	"area.cpp";
	"container.cpp";
}

Library( "core", Compile( srcs ) );

