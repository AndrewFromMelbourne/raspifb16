#!/usr/bin/env python3
""" print_image """

import argparse

from PIL import Image
from numpy import asarray
from pathlib import Path

# ================================================================================

def hex_fill(value, length):
    return '0x' + hex(value)[2:].zfill(length)

# ================================================================================

def main():
    """main"""

    # ----------------------------------------------------------------------------

    parser = argparse.ArgumentParser()
    parser.add_argument('name', help='image file name')
    args = parser.parse_args()

    # ----------------------------------------------------------------------------

    image = Image.open(args.name)

    image_rgba = image.convert('RGBA')
    data = asarray(image_rgba)

    name_stem = Path(args.name).stem
    name = f"{name_stem}Image"
    length = image.width * image.height
    print(f"std::vector<uint32_t> {name} = ")
    print("{", end='')

    codeWidth = 10
    #codeWidth = image.width

    #while codeWidth > 16:
    #    codeWidth = codeWidth // 2

    index = 0

    for i in range(image.width):
        x = image.width - 1 - i
        for j in range(image.height):

            if ((index % codeWidth) == 0):
                print("")
                print("    ", end='')
            index += 1
            y = j
            pixel = data[y][x]

            r = pixel[0]
            g = pixel[1]
            b = pixel[2]
            a = pixel[3]

            value = b | (g << 8) | (r << 16)

            print(hex_fill(value, 8) + ", ", end='')

    print("")
    print("};")

# ================================================================================

if __name__ == '__main__':
    main()
