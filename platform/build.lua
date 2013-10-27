
srcs = {
	"system.cpp";
	"screen.cpp";
	"window.cpp";
	"timer.cpp";
	"keyboard.cpp";
	"mouse.cpp";
	"font_manager.cpp";
	"dispatcher.cpp";
}

Library( "platform", Compile( srcs ), LinkLibs( "draw" ) )

if System() == "Linux" then
	SubDir( "xcb" )
elseif System() == "Darwin" then
	SubDir( "xcb" )
	SubDir( "cocoa" )
end

