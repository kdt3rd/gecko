
srcs = {
	"reaction.cpp";
	"button.cpp";
}

Library( "reaction", Compile( srcs ), LinkLibs( "layout" ) )

