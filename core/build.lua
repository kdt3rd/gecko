
Include( source_dir )

srcs = {
	"contract.cpp";
	"rectangle.cpp";
	"line.cpp";
	"area.cpp";
	"container.cpp";
}

Library( "core", Compile( srcs ) );

