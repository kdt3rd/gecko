
DefaultBuildType( "build" )

BuildDir( "build", "build" )
BuildDir( "debug", "debug" )
BuildDir( "release", "release" )
BuildDir( "mingw", "mingw" )

if Building( "mingw" ) then
	CrossCompile( "x86_64", "Windows" )
end

if System() == "Linux" then
--	Variable( "cxx", "g++" )
--	Variable( "cc", "gcc" )
--	Variable( "ld", "g++" )
	Variable( "cxx", "clang++" )
	Variable( "cc", "clang" )
	Variable( "ld", "clang++" )
	Variable( "binld", "ld" )
	Variable( "ar", "ar" )
	Variable( "ranlib", "ranlib" )
elseif System() == "Darwin" then
	assert( not Building( "mingw" ), "MingW build only under linux" )
	Variable( "cxx", "clang++" )
	Variable( "cc", "clang" )
	Variable( "ld", "clang++" )
	Variable( "binld", "ld" )
	Variable( "ar", "ar" )
	Variable( "ranlib", "ranlib" )
	CXXFlags( "--stdlib=libc++" )
	LDFlags( "--stdlib=libc++" )
elseif System() == "Windows" then
	if Building( "mingw" ) then
		Variable( "cxx", "i686-w64-mingw32-g++" )
		Variable( "cc", "i686-w64-mingw32-gcc" )
		Variable( "ld", "i686-w64-mingw32-g++" )
		Variable( "binld", "i686-w64-mingw32-ld" )
		Variable( "ar", "i686-w64-mingw32-ar" )
		Variable( "ranlib", "i686-w64-mingw32-ranlib" )
--		Variable( "cxx", "i486-mingw32-g++" )
--		Variable( "cc", "i486-mingw32-gcc" )
--		Variable( "ld", "i486-mingw32-g++" )
		LDFlags( "-static-libgcc", "-static-libstdc++" )
	else
		error( "Windows compile not supported yet" )
	end
end

Variable( "moc", "moc-qt4" )
Variable( "rcc", "rcc" )
Variable( "luac", "luac" )
Variable( "doxygen", "doxygen" )
Variable( "mkdir", "mkdir" )
Variable( "copy", "cp" )

CFlags( "-msse", "-msse2", "-msse3" )
CFlags( "-flax-vector-conversions" )
if System() == "Linux" then
	CFlags( "-fPIC" )
end
CXXFlags( "-msse", "-msse2", "-msse3" )
CXXFlags( "-flax-vector-conversions" )
CXXFlags( "--std=c++11" )

-- C/C++ warnings
Warning( "all", "extra", "no-unused-parameter", "init-self", "comment", "cast-align", "switch", "format", "multichar", "missing-braces", "parentheses", "pointer-arith", "sign-compare", "return-type", "write-strings", "cast-align" )

-- C warnings
CWarning( "unused" )

-- C++ warnings
CXXWarning( "unused", "overloaded-virtual", "no-ctor-dtor-privacy", "non-virtual-dtor", "sign-promo", "missing-field-initializers" )

if Building( "debug" ) then
	CFlags( "-ggdb" ) --, "-fvar-tracking-uninit", "-fbounds-check", "-fcheck-data-deps" )
--	Library = SharedLibrary
--	LinkLibs = LinkSharedLibs
	Library = StaticLibrary
	LinkLibs = LinkStaticLibs
end

if Building( "build" ) then
	CFlags( "-O3" )
	Definition( "NDEBUG" )
	Library = StaticLibrary
	LinkLibs = LinkStaticLibs
end

if Building( "mingw" ) then
	CFlags( "-m32", "-O3" )
	Definition( "NDEBUG" )
	LDFlags( "-m32" )
	Library = StaticLibrary
	LinkLibs = LinkStaticLibs
end

if Building( "release" ) then
	CFlags( "-O3" )
	Definition( "NDEBUG" )
	Variable( "luaflags", "-s" ) -- Strip lua files
	Definition( "QT_NO_DEBUG" )
	Library = StaticLibrary
	LinkLibs = LinkStaticLibs
end

Definition( "__STDC_LIMIT_MACROS", "_FILE_OFFSET_BITS=64" )

LDFlags( "-L" .. build_dir .. "/lib" )

Include( source_dir .. "/lib" )

BOTAN_FLAGS, BOTAN_INCLUDE, BOTAN_LIBS = Package( "botan-1.10" )
--SDL_FLAGS, SDL_INCLUDE, SDL_LIBS = Package( "sdl2" )

if System() == "Linux" then
	CAIRO_FLAGS, CAIRO_INCLUDE, CAIRO_LIBS = Package( "cairo", "cairo-xcb" )
	XCB_FLAGS, XCB_INCLUDE, XCB_LIBS = Package( "xcb", "xcb-keysyms" )
	FREETYPE_FLAGS, FREETYPE_INCLUDE, FREETYPE_LIBS = Package( "freetype2" )
	FONTCONFIG_FLAGS, FONTCONFIG_INCLUDE, FONTCONFIG_LIBS = Package( "fontconfig" )
elseif System() == "Darwin" then
	CAIRO_FLAGS, CAIRO_INCLUDE, CAIRO_LIBS = Package( "cairo", "cairo-quartz" )
	COCOA_FLAGS, COCOA_INCLUDE, COCOA_LIBS = Package( "Cocoa" )
end

