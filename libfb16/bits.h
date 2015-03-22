//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Andrew Duncan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------

#ifndef BITS_H
#define BITS_H

//-------------------------------------------------------------------------

typedef enum
{
    B00000000B = 0x00,
    B00000001B = 0x01,
    B00000010B = 0x02,
    B00000011B = 0x03,
    B00000100B = 0x04,
    B00000101B = 0x05,
    B00000110B = 0x06,
    B00000111B = 0x07,
    B00001000B = 0x08,
    B00001001B = 0x09,
    B00001010B = 0x0A,
    B00001011B = 0x0B,
    B00001100B = 0x0C,
    B00001101B = 0x0D,
    B00001110B = 0x0E,
    B00001111B = 0x0F,
    B00010000B = 0x10,
    B00010001B = 0x11,
    B00010010B = 0x12,
    B00010011B = 0x13,
    B00010100B = 0x14,
    B00010101B = 0x15,
    B00010110B = 0x16,
    B00010111B = 0x17,
    B00011000B = 0x18,
    B00011001B = 0x19,
    B00011010B = 0x1A,
    B00011011B = 0x1B,
    B00011100B = 0x1C,
    B00011101B = 0x1D,
    B00011110B = 0x1E,
    B00011111B = 0x1F,
    B00100000B = 0x20,
    B00100001B = 0x21,
    B00100010B = 0x22,
    B00100011B = 0x23,
    B00100100B = 0x24,
    B00100101B = 0x25,
    B00100110B = 0x26,
    B00100111B = 0x27,
    B00101000B = 0x28,
    B00101001B = 0x29,
    B00101010B = 0x2A,
    B00101011B = 0x2B,
    B00101100B = 0x2C,
    B00101101B = 0x2D,
    B00101110B = 0x2E,
    B00101111B = 0x2F,
    B00110000B = 0x30,
    B00110001B = 0x31,
    B00110010B = 0x32,
    B00110011B = 0x33,
    B00110100B = 0x34,
    B00110101B = 0x35,
    B00110110B = 0x36,
    B00110111B = 0x37,
    B00111000B = 0x38,
    B00111001B = 0x39,
    B00111010B = 0x3A,
    B00111011B = 0x3B,
    B00111100B = 0x3C,
    B00111101B = 0x3D,
    B00111110B = 0x3E,
    B00111111B = 0x3F,
    B01000000B = 0x40,
    B01000001B = 0x41,
    B01000010B = 0x42,
    B01000011B = 0x43,
    B01000100B = 0x44,
    B01000101B = 0x45,
    B01000110B = 0x46,
    B01000111B = 0x47,
    B01001000B = 0x48,
    B01001001B = 0x49,
    B01001010B = 0x4A,
    B01001011B = 0x4B,
    B01001100B = 0x4C,
    B01001101B = 0x4D,
    B01001110B = 0x4E,
    B01001111B = 0x4F,
    B01010000B = 0x50,
    B01010001B = 0x51,
    B01010010B = 0x52,
    B01010011B = 0x53,
    B01010100B = 0x54,
    B01010101B = 0x55,
    B01010110B = 0x56,
    B01010111B = 0x57,
    B01011000B = 0x58,
    B01011001B = 0x59,
    B01011010B = 0x5A,
    B01011011B = 0x5B,
    B01011100B = 0x5C,
    B01011101B = 0x5D,
    B01011110B = 0x5E,
    B01011111B = 0x5F,
    B01100000B = 0x60,
    B01100001B = 0x61,
    B01100010B = 0x62,
    B01100011B = 0x63,
    B01100100B = 0x64,
    B01100101B = 0x65,
    B01100110B = 0x66,
    B01100111B = 0x67,
    B01101000B = 0x68,
    B01101001B = 0x69,
    B01101010B = 0x6A,
    B01101011B = 0x6B,
    B01101100B = 0x6C,
    B01101101B = 0x6D,
    B01101110B = 0x6E,
    B01101111B = 0x6F,
    B01110000B = 0x70,
    B01110001B = 0x71,
    B01110010B = 0x72,
    B01110011B = 0x73,
    B01110100B = 0x74,
    B01110101B = 0x75,
    B01110110B = 0x76,
    B01110111B = 0x77,
    B01111000B = 0x78,
    B01111001B = 0x79,
    B01111010B = 0x7A,
    B01111011B = 0x7B,
    B01111100B = 0x7C,
    B01111101B = 0x7D,
    B01111110B = 0x7E,
    B01111111B = 0x7F,
    B10000000B = 0x80,
    B10000001B = 0x81,
    B10000010B = 0x82,
    B10000011B = 0x83,
    B10000100B = 0x84,
    B10000101B = 0x85,
    B10000110B = 0x86,
    B10000111B = 0x87,
    B10001000B = 0x88,
    B10001001B = 0x89,
    B10001010B = 0x8A,
    B10001011B = 0x8B,
    B10001100B = 0x8C,
    B10001101B = 0x8D,
    B10001110B = 0x8E,
    B10001111B = 0x8F,
    B10010000B = 0x90,
    B10010001B = 0x91,
    B10010010B = 0x92,
    B10010011B = 0x93,
    B10010100B = 0x94,
    B10010101B = 0x95,
    B10010110B = 0x96,
    B10010111B = 0x97,
    B10011000B = 0x98,
    B10011001B = 0x99,
    B10011010B = 0x9A,
    B10011011B = 0x9B,
    B10011100B = 0x9C,
    B10011101B = 0x9D,
    B10011110B = 0x9E,
    B10011111B = 0x9F,
    B10100000B = 0xA0,
    B10100001B = 0xA1,
    B10100010B = 0xA2,
    B10100011B = 0xA3,
    B10100100B = 0xA4,
    B10100101B = 0xA5,
    B10100110B = 0xA6,
    B10100111B = 0xA7,
    B10101000B = 0xA8,
    B10101001B = 0xA9,
    B10101010B = 0xAA,
    B10101011B = 0xAB,
    B10101100B = 0xAC,
    B10101101B = 0xAD,
    B10101110B = 0xAE,
    B10101111B = 0xAF,
    B10110000B = 0xB0,
    B10110001B = 0xB1,
    B10110010B = 0xB2,
    B10110011B = 0xB3,
    B10110100B = 0xB4,
    B10110101B = 0xB5,
    B10110110B = 0xB6,
    B10110111B = 0xB7,
    B10111000B = 0xB8,
    B10111001B = 0xB9,
    B10111010B = 0xBA,
    B10111011B = 0xBB,
    B10111100B = 0xBC,
    B10111101B = 0xBD,
    B10111110B = 0xBE,
    B10111111B = 0xBF,
    B11000000B = 0xC0,
    B11000001B = 0xC1,
    B11000010B = 0xC2,
    B11000011B = 0xC3,
    B11000100B = 0xC4,
    B11000101B = 0xC5,
    B11000110B = 0xC6,
    B11000111B = 0xC7,
    B11001000B = 0xC8,
    B11001001B = 0xC9,
    B11001010B = 0xCA,
    B11001011B = 0xCB,
    B11001100B = 0xCC,
    B11001101B = 0xCD,
    B11001110B = 0xCE,
    B11001111B = 0xCF,
    B11010000B = 0xD0,
    B11010001B = 0xD1,
    B11010010B = 0xD2,
    B11010011B = 0xD3,
    B11010100B = 0xD4,
    B11010101B = 0xD5,
    B11010110B = 0xD6,
    B11010111B = 0xD7,
    B11011000B = 0xD8,
    B11011001B = 0xD9,
    B11011010B = 0xDA,
    B11011011B = 0xDB,
    B11011100B = 0xDC,
    B11011101B = 0xDD,
    B11011110B = 0xDE,
    B11011111B = 0xDF,
    B11100000B = 0xE0,
    B11100001B = 0xE1,
    B11100010B = 0xE2,
    B11100011B = 0xE3,
    B11100100B = 0xE4,
    B11100101B = 0xE5,
    B11100110B = 0xE6,
    B11100111B = 0xE7,
    B11101000B = 0xE8,
    B11101001B = 0xE9,
    B11101010B = 0xEA,
    B11101011B = 0xEB,
    B11101100B = 0xEC,
    B11101101B = 0xED,
    B11101110B = 0xEE,
    B11101111B = 0xEF,
    B11110000B = 0xF0,
    B11110001B = 0xF1,
    B11110010B = 0xF2,
    B11110011B = 0xF3,
    B11110100B = 0xF4,
    B11110101B = 0xF5,
    B11110110B = 0xF6,
    B11110111B = 0xF7,
    B11111000B = 0xF8,
    B11111001B = 0xF9,
    B11111010B = 0xFA,
    B11111011B = 0xFB,
    B11111100B = 0xFC,
    B11111101B = 0xFD,
    B11111110B = 0xFE,
    B11111111B = 0xFF
} BITS_T;

//-------------------------------------------------------------------------

#endif
