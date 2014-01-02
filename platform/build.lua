
local platform = {
	incs = {
	};

	srcs = {
		"platform.cpp";
		"system.cpp";
		"screen.cpp";
		"window.cpp";
		"timer.cpp";
		"keyboard.cpp";
		"mouse.cpp";
		"font_manager.cpp";
		"dispatcher.cpp";
	};

	libs = {
		"draw";
	};

	syslibs = {
	};
}

function Platform( p )
	if p.incs then
	for i, s in ipairs( p.incs ) do
		table.insert( platform.incs, s )
	end
	end

	if p.srcs then
	for i, s in ipairs( p.srcs ) do
		table.insert( platform.srcs, p.name .. "/" .. s )
	end
	end

	if p.libs then
	for i, s in ipairs( p.libs ) do
		table.insert( platform.libs, s )
	end
	end

	if p.syslibs then
	for i, s in ipairs( p.syslibs ) do
		table.insert( platform.syslibs, s )
	end
	end
end

vars = { Platform = Platform }

if System() == "Linux" then
	table.insert( platform.srcs, "linux.cpp" )
	SubDir( "fc", vars )
	SubDir( "xcb", vars )
	SubDir( "xlib", vars )
elseif System() == "Darwin" then
	table.insert( platform.srcs, "darwin.cpp" )
	SubDir( "xcb", vars )
	SubDir( "cocoa", vars )
elseif System() == "Windows" then
	table.insert( platform.srcs, "windows.cpp" )
	SubDir( "mswin", vars )
end
SubDir( "dummy", vars )

Include( platform.incs )

Library( "platform", Compile( platform.srcs ), LinkLibs( platform.libs ), LinkSys( platform.syslibs ) )

