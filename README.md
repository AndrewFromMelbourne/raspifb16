# raspifb16
This repository contains a C++ convenience library for directly manipulating
the Linux Framebuffer. It was written specifically to access the 16 bit per
pixel framebuffers found on the accessory LCD displays on the Raspberry Pi.

# libraries

You will need to install libbsd-dev, libdrm-dev, libfreetype-dev, libiw-dev

	sudo apt-get install libbsd-dev libdrm-dev libfreetype-dev libiw-dev

# libfb16
The library itself.

# tests
## test
A very simple test program that displays text and simple graphics

## testCircle
Test circle drawing functions

## testColour
Displays hues at different brightness and saturation.

## testColourBlend
Test blending a blue background with a red foreground at alpha value from 0 to 255.

## testDoubleBuffer
Test DRM/KMS double buffering by displaying one red and one greem buffer. **WARNING:** causes a strobing effect.

## testft and testft2
Tests programs for truetype fonts (Requires Freetype2).

## testPutimage
Test putImage function overlapping screen edge.

## testResize
Test image resizing using scale-upi, nearest neighbour, bilinear interpolation and Lanczos3 interpolation.

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

# The down side of double buffering

I created a bug by enabling double buffering in the KMS/DRM dumb buffer. When you draw persisently and directly to the frame buffer you need to make sure that you draw to both front and back buffers. I have added a convenience function clearBuffer() that will clear the front and back buffers for the dumb buffer. It adds a small amount of overhead for the frame buffer.
