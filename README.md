# raspifb16
This repository contains a C++ convenience library for directly manipulating
the Linux Framebuffer. It was written specifically to access the 16 bit per
pixel framebuffers found on the accessory LCD displays on the Raspberry Pi.

# libraries

You will need to install libbsd-dev

   sudo apt-get install libbsd-dev

# libfb16
The library itself.

# test
A very simple test program that displays text on /dev/fd1

# raspinfo
A program to display Raspberry Pi specific system information directly on
the framebuffer.

# build
    cd raspifb16
    mkdir build
    cd build
    cmake ..
    make

