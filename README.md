# raspifb16
This repository contains a C++ convenience library for directly manipulating
the Linux Framebuffer. It was written specifically to access the 16 bit per
pixel framebuffers found on the accessory LCD displays on the Raspberry Pi.

# libraries

You will need to install libbsd-dev, libdrm-dev, libfreetype-dev, libfmt-dev, libiw-dev

	sudo apt-get install libbsd-dev libdrm-dev libfreetype-dev libfmt-dev libiw-dev

# libfb16
The library itself.

# tests
## test
A very simple test program that displays text and simple graphics

## testCircle
Test circle drawing functions

## testColour
Displays a hues at different brightness and saturation.

## testft and testft2
Tests programs for truetype fonts (Requires Freetype2).

## testPutimage
Test putImage function overlapping screen edge.

# boxworld
A version of Boxworld or Sokoban (Requires a joystick).

# fbpipe
Display text from standard input.

# puzzle-15
A sliding puzzle (Requires a joystick).

# splash
A QOI image viewer.

# raspinfo
A program to display Raspberry Pi specific system information directly on
the framebuffer.

# wifiscan
A wifi access point scanner.

# build

	cd raspifb16
	mkdir build
	cd build
	cmake ..
	make

