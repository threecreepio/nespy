# NESpy Input Reader

https://github.com/threecreepio/nespy

Windows application that can read and display NES and SNES controllers.

There is also some support for Keyboards and USB Joypads.

## Configuring

There is an NESpy.ini file which contains all settings for the program.

If you want to use another file, you can just write any ini file and drop it onto the NESpy.exe application when you want to start with that configuration (for example if you have a separate NES and SNES profile.)

In the images folder there is an image called "0.png" which is the base image that is always displayed, then there are numbered images for each supported button. They can be transparent to show other states underneath them, but they must all be exactly the same size. You can use transparency of course.

When the program starts it writes to an NESpy.log file with anything that might happen during runtime. If the program crashes, look there and hopefully it wrote something useful!

## Building with Visual Studio

To build the project with Visual Studio, just open the folder and you should be able to compile.

## Building with Ninja + Clang

To build with Clang, make sure to install LLVM, Cmake and Clang (make sure to select to add LLVM to path during the install):

    winget install -i Ninja-build.Ninja LLVM.LLVM CMake

Navigate into the folder containing the code and write:

    cmake --preset clang -B build

    cmake --build build

If all worked as it should, there will be a "build" folder containing an "out" folder which will have your NESpy build.

After making changes you can write `cmake --build build` again to recompile.

Have fun!

/threecreepio
