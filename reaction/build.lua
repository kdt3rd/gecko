
srcs = {
	"reaction.cpp";
	"passive.cpp";
	"button.cpp";
	"slider.cpp";
}

Library( "reaction", Compile( srcs ), LinkLibs( "layout" ) )

