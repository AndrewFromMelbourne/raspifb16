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

    image_rgba = image.convert('RGB')
    data = asarray(image_rgba)

    name_stem = Path(args.name).stem
    name = f"{name_stem}Image"
    length = image.width * image.height
    print(f"std::vector<uint16_t> {name} = ")
    print("{", end='')

    codeWidth = 15
    index = 0

    for j in range(image.height):
        for i in range(image.width):

            if ((index % codeWidth) == 0):
                print("")
                print("    ", end='')
            index += 1
            pixel = data[j][i]

            r = (pixel[0] >> 3) & 0x1F
            g = (pixel[1] >> 2) & 0x3F
            b = (pixel[2] >> 3) & 0x1F

            value = b | (g << 5) | (r << 11)

            print(hex_fill(value, 4) + ", ", end='')

    print("")
    print("};")

# ================================================================================

if __name__ == '__main__':
    main()
