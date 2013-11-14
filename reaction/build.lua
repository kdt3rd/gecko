
srcs = {
	"reaction.cpp";
	"passive.cpp";
	"button.cpp";
}

Library( "reaction", Compile( srcs ), LinkLibs( "layout" ) )

