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

