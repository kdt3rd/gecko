
Include( BOTAN_INCLUDE )

srcs = {
	"data_writer.cpp";
	"data_reader.cpp";
	"hash256.cpp";
	"repository.cpp";
}

Library( "store", Compile( srcs ), LinkSys( BOTAN_LIBS ) )

