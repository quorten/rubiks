This is a very simple Rubik's Cube program that is not necessarily as
efficient in rendering the Rubik's Cube as it could be, especially
when when bug workarounds are enabled.

In order to build this program, you must have a working GTK+ 2
environment with pkgconfig and GtkGLExt installed (along with the
equivalent development packages for GTK+ 2 and GtkGLExt).

To build this program, simply combine the appropriate makefile
definitions with "Makefile.in" and output the result to "Makefile".
Then you can make the program as usual.

Note that the program may crash on your system or behave unusually
when using the keyboard to rotate the Rubik's Cube.  For these two
problems, try using the preprocessor definitions VARRAY_BUG and
IDLE_BUG respectively.

As a side note, this program may seem like a crazy disaster of
multiple coding styles meshed together in an unintelligible way.  The
reason why the code is written in two different styles throughout the
program is because the old code base used to be a Windows-only OpenGL
program, while the new code base was made platform-independent by
switching the wgl and Windows code to GtkGLExt and GTK+ 2.

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
