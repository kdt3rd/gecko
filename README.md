gecko
=====

Gecko is a c++11 framework with a number of applications. It has
several libraries which serve as the basis for an (envisioned) suite
of applications.

This utilizes c++11 and relatively modern STL components, but does not
live on the bleeding edge of compiler support. It should compile with
a compiler equivalent to gcc 4.8 or newer.

Libraries
---------

base - As the name indicates, a base library with utility stuff
color - library to implement color description and color science math
draw - core OpenGL drawing utilities
gl - abstraction around OpenGL
gui - widgets [currently inactive]
image - image processing operations
imgproc - start to a compiler language similar in vein to halide
layout - layout engine for GUI
media - file I/O library for all manner of media
net - simple networking classes
platform - abstraction of xlib / xcb / wayland / cocoa / windows window/events
script - font loading / rendering abstraction
sqlite - classes around a sqlite database backend
utf - utf support
viewer - opengl viewer widget [currently inactive]
web - classes for handling web requests for building json/rpc enabled apps

Applications
------------

image_script - wrapper shell around running image processing
transcode - convert media files
denoise - a sample application that can be used in a variety of ways
to denoise an image sequence

Unit Testing
------------

The tests folder houses unit test wrappers that can be run by typing
"make test"

Compiling
---------

gecko uses constructor (https://github.com/kdt3rd/constructor) as a
makefile (or a ninja file if ninja is found) generator, much like
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

