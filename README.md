gecko
=====

![](https://github.com/kdt3rd/gecko/workflows/CI%20Build/badge.svg)

Gecko is a c++11 framework. Contained in it are support libraries, a
GUI toolkit, media I/O framework and image processing toolbox based on
years of production experience in the visual effects and post
production community.

Built on those libraries will be a number of applications.

Every attempt will be made to pay attention to the VFX Reference
Platform and not exceed the features of C++ or compiler support for
the current year. However, the project leaders do not have the support
infrastructure to test all variants of platforms.

It is primarily being written to run on a linux platform, with an
Apple laptop being the second, and finally Windows support. Windows
support will primarily be tested via cross-compiling from windows
using the MingW infrastructure then running under wine. Any other
testing is of course welcome.

Libraries
---------

- base: As the name indicates, a base library with utility stuff
- color: library to implement color description and color science math
- draw: core OpenGL drawing utilities
- gl: abstraction around OpenGL
- gui: widgets [active development]
- image: image processing operations
- imgproc: start to a compiler language similar in vein to halide
- layout: layout engine for GUI
- media: file I/O library for all manner of media
- net: simple networking classes
- platform: abstraction of xlib / xcb / wayland / cocoa / windows window/events
- script: font loading / rendering abstraction
- sqlite: classes around a sqlite database backend
- utf: utf support
- viewer: opengl viewer widget [currently inactive]
- web: classes for handling web requests for building json/rpc enabled apps

Applications
------------

- image_script: wrapper shell around running image processing
- transcode: convert media files
- denoise: a sample application that can be used in a variety of ways
to denoise an image sequence

Unit Testing
------------

The tests folder houses unit test wrappers that can be run by typing
"make test"

Compiling
---------

gecko by default uses two additional components to compile:
ninja (https://github.com/ninja-build/ninja)
constructor (https://github.com/kdt3rd/constructor)

Library dependencies are kept to a minimum - gecko attempts to keep a
clean set of dependencies to make sure that it can easily be a
friendly architecture for use in corporate environments where GPL code
can be tricky. But there are libraries that gecko depends on - it
doesn't make sense to reinvent everything, just the components that
are interesting to play with.

gecko currently uses OpenGL for UI components, and a set of media
libraries to read images (currently, just OpenEXR and TIFF -
OpenImageIO does not currently allow the kind of media and frame
buffer access desired). See libs/media/construct. Additionally, it
uses zlib to compress the UTF tables in libs/utf.

constructor is a makefile / ninja build file generator, much like
cmake. Why not use cmake? Well, constructor attempts to simplify the
syntax for cmake, but more importantly allows a cleaner specification
for custom compilation toolchains and options in a cross-platform
environment - support for "if it's this OS, or if this library is
present" type questions have a new approach to incorporating those
conditional options.

If you find that constructor does not work out of the box on your
O.S. flavor, do not be surprised. If you look at
config/MingW64.construct and can't figure out how to define a
toolchain that will work, please file an issue report or otherwise
contact the developer.

As mentioned above, gecko uses C++11, although refrains from using all
features of C++11, although which components are used will probably
change in the future as support continues to percolate through the
"stable" releases of operating systems. It has been confirmed to
compile on gcc 4.8 and newer gcc, as well as the recent versions of
Xcode on the Mac. It has largely only been tested compiled as a set of
static libraries. Dynamic libraries (.so, .dylib, .dll) should be
possible if desired, but no attempt at dynamic library versioning has
been put in place, so with the issues of C++ and API stability, it is
easier and safer to just link things statically.

Windows support: constructor provides support for cross compiling, and
  a brief pass was made to compile under Windows. While the intent is
  to have gecko be a fully cross-platform toolset, at this stage,
  little is being done for windows.

Installing
----------

No attempt at installing the resulting libraries or binaries has been made.

