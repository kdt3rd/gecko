
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
	SubDir( "fc" )
	SubDir( "xcb" )
	SubDir( "xlib" )
elseif System() == "Darwin" then
	SubDir( "xcb" )
	SubDir( "cocoa" )
elseif System() == "Windows" then
	SubDir( "mswin" )
end
SubDir( "dummy" )

