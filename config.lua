
DefaultBuildType( "build" )

BuildDir( "build", "build" )
BuildDir( "debug", "debug" )
BuildDir( "release", "release" )

if System() == "Linux" then
	Variable( "cxx", "g++" )
	Variable( "cc", "gcc" )
	Variable( "ld", "g++" )
elseif System() == "Darwin" then
	Variable( "cxx", "/usr/local/bin/g++" )
	Variable( "cc", "/usr/local/bin/gcc" )
	Variable( "ld", "/usr/local/bin/g++" )
end

Variable( "binld", "ld" )
Variable( "ar", "ar" )
Variable( "moc", "moc-qt4" )
Variable( "rcc", "rcc" )
Variable( "luac", "luac" )
Variable( "doxygen", "doxygen" )

CFlags( "-msse", "-msse2", "-msse3" )
CFlags( "-flax-vector-conversions" )
CFlags( "-fPIC", "-Wl,-export-dynamic" )
CXXFlags( "-msse", "-msse2", "-msse3" )
CXXFlags( "-flax-vector-conversions" )
CXXFlags( "--std=c++11" )

-- C/C++ warnings
Warning( "all", "extra", "no-unused-parameter", "init-self", "comment", "cast-align", "switch", "format", "multichar", "missing-braces", "parentheses", "pointer-arith", "sign-compare", "return-type", "write-strings", "cast-align" )

-- C warnings
CWarning( "unused" )

-- C++ warnings
CXXWarning( "unused", "overloaded-virtual", "no-ctor-dtor-privacy", "non-virtual-dtor", "pmf-conversions", "sign-promo", "missing-field-initializers" )

if Building( "debug" ) then
	CFlags( "-ggdb", "-fvar-tracking-uninit", "-fbounds-check", "-fcheck-data-deps" )
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
SDL_FLAGS, SDL_INCLUDE, SDL_LIBS = Package( "sdl2" )
XCB_FLAGS, XCB_INCLUDE, XCB_LIBS = Package( "xcb", "xcb-xinput", "xcb-keysyms" )
CAIRO_FLAGS, CAIRO_INCLUDE, CAIRO_LIBS = Package( "cairo" )
FREETYPE_FLAGS, FREETYPE_INCLUDE, FREETYPE_LIBS = Package( "freetype2" )

