Rubik's Cube
============

This is a very simple OpenGL Rubik's Cube program that is not
necessarily as efficient in rendering the Rubik's Cube as it could be,
especially when when bug workarounds are enabled.

There are multiple ways to build this program, depending on which
dependencies you want to use:

* Windows API + WGL + OpenGL 1.1, this is the dependency-minimal way
  to compile the software for legacy Microsoft Windows.  Works well on
  Windows 95/98/2000/ME/NT/XP.

  Use `make -f Makefile.wd CROSS=i686-w64-mingw32-` to compile and set
  the CROSS variable accordingly for cross-compiling (or empty for
  native compiling).

* libX11 + GLX + OpenGL 1.1, this is the dependency-minimal way to
  compile the software for legacy Unix.  It also still works
  reasonably well on modern Unix, at most requiring only a
  compatibility layer dependency (XQuartz, XWayland) similar to the
  addition of a windowing toolkit dependency, but without the breaking
  changes that come about in newer toolkit versions.

  Use `make -f Makefile.xd` to compile.

* GTK+ 2 + GtkGLExt + OpenGL 1.1, this was a good way to write
  cross-platform GUI software for a good chunk of time from around the
  years 2005 - 2015, but due to the scope of breaking changes in GTK+
  3, GTK+ is no longer liked as a platform by application developers.

  To compile for Windows: `cat mingw.defs Makefile.in >Makefile && make`

  To compile for Unix: `cat posix.defs Makefile.in >Makefile && make`

On Debian-based systems (Ubuntu, Raspbian, etc.), make sure you have
the required development packages installed, these are the ones that
have been specifically tested and known to work:

* libx11-dev
* libgl1-mesa-dev
* libglu1-mesa-dev
* pkg-config (optional)
* libgtk2.0-dev (optional)
* libgtkglext1-dev (optional)

Note that the program may behave unusually when using the keyboard to
rotate the Rubik's Cube, though this only ever happened in my testing
on Ubuntu 11.04.  For this problem, try using the preprocessor
definitions IDLE_BUG.

As a side note, this program may seem like a crazy disaster of
multiple coding styles meshed together in an unintelligible way.  The
reason why the code is written in two different styles throughout the
program is because the original code base used to be a Windows-only
OpenGL program, while the new code base was written for Unix, first as
GtkGLExt and GTK+ 2 for a short semblance of cross-platform
capability, and finally adding a GLX implementation for
dependency-minimal compiles on Unix.  Why did it take so long for me
to add the GLX implementation?  I'll tell you why: Do you have any
idea how hard it is to navigate the documentation on Xlib?  It's a
total pain compared to Windows API, GTK+, and anything else modern!

How To Use
----------

Using this Rubik's Cube program is simple.  You can rotate the Rubik's
Cube by clicking and dragging the right mouse button or by using the
arrow keys.  Holding down one of the control keys will cause the
Rubik's Cube to rotate four times as fast when using the arrow keys.

To twist the Rubik's Cube, you can click and drag on a face to rotate
that face.  To use the keyboard to control the Rubik's Cube, use the
following keys:

Setting the current face:  
'q': white  
'a': yellow  
'w': green  
's': blue  
'e': red  
'd': orange

'r': Rotate the current face counterclockwise  
'f': Rotate the current face clockwise  
'z': Align the Rubik's Cube (if you were using the mouse to twist)  
'n': Wastefully throw away your perfectly good Rubik's Cube and buy a
     new one.  Was it really that bad that you HAD to throw it away?
