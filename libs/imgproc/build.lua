
local srcs =
{
	"token.cpp";
	"expr.cpp";
	"message.cpp";
	"parser.cpp";
}

Library( "imgproc", Compile( srcs ), LinkLibs( "utf", "base" ) );
