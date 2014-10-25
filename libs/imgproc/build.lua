
local srcs =
{
	"token.cpp";
	"expr.cpp";
	"message.cpp";
	"parser.cpp";
	"operators.cpp";
	"expr_parser.cpp";
--	"cpp_generator.cpp";
	"function.cpp";
	"type.cpp";
	"unifier.cpp";
	"environment.cpp";
}

Library( "imgproc", Compile( srcs ), LinkLibs( "utf", "base" ) );
