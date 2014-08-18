
local srcs =
{
	"token.cpp";
	"expr.cpp";
	"message.cpp";
	"parser.cpp";
	"operators.cpp";
	"expr_parser.cpp";
}

Library( "imgproc", Compile( srcs ), LinkLibs( "utf", "base" ) );
