gecko
=====

Gecko is a c++11 application framework. It has several libraries, and will end
up with several applications utilizing those libraries.

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

Unit Testing
------------

tests houses unit test wrappers that can be run by typing "make test"

Compiling
---------

gecko uses constructor (https://github.com/kdt3rd/constructor) as a makefile
generator, much like cmake, only simpler.

gecko uses C++11, although refrains from using all features of C++11, although
that may change in teh future. It has been confirmed to compile on gcc 4.8 and
newer gcc, as well as the recent versions of Xcode on the Mac.

Windows support: constructor provides support for cross compiling, and a brief
  pass was made to compile under Windows. While the intent is to have gecko
  be a fully cross-platform toolset, at this stage, little is being done for
  windows.

Installing
----------

No attempt at installing the resulting libraries or binaries has been made.

