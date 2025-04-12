# NESpy Input Reader

https://github.com/threecreepio/nespy

Windows application that can read and display NES and SNES controllers.

There is also some support for Keyboards and USB Joypads.

## Configuring

There is an NESpy.ini file which contains all settings for the program.

If you want to use another file, you can just write any ini file and drop it onto the NESpy.exe application when you want to start with that configuration (for example if you have a separate NES and SNES profile.)

In the images folder there is an image called "0.png" which is the base image that is always displayed, then there are numbered images for each supported button. They can be transparent to show other states underneath them, but they must all be exactly the same size. You can use transparency of course.

When the program starts it writes to an NESpy.log file with anything that might happen during runtime. If the program crashes, look there and hopefully it wrote something useful!

## Building

The project is Windows-only, and requires CMake and a C compiler (LLVM Clang, TDM GCC, Visual Studio.)

https://cmake.org/download/

Create a build subdirectory, and inside it write:

cmake -DCMAKE_BUILD_TYPE=Release ..

This will generate build files for your system. After that you'll want to use your C compiler to build the executable.

Have fun!

/threecreepio
