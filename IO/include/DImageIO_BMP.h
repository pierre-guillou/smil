/*
 * Copyright (c) 2011, Matthieu FAESSEL and ARMINES
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _D_IMAGE_IO_BMP_H
#define _D_IMAGE_IO_BMP_H

#include "DImageIO.h"
#include <fstream>
#include <iostream> 

#ifndef DWORD
#define DWORD unsigned long
#define WORD unsigned short
#define LONG long
#endif

#define BITMAP_ID 0x4D42        // the universal bitmap ID

#pragma pack(push, 1) // force the compiler to pack the structs
struct bmpFileHeader
{
    UINT16 bfType;  // file type
    UINT32 bfSize;  // size in bytes of the bitmap file
    UINT16 bfReserved1;  // reserved; must be 0
    UINT16 bfReserved2;  // reserved; must be 0
    UINT32 bfOffBits;  // offset in bytes from the bitmapfileheader to the bitmap bits
};

struct bmpInfoHeader
{
    UINT32 biSize;  // number of bytes required by the struct
    UINT32 biWidth;  // width in pixels
    UINT32 biHeight;  // height in pixels
    UINT16 biPlanes; // number of color planes, must be 1
    UINT16 biBitCount; // number of bit per pixel
    UINT32 biCompression;// type of compression
    UINT32 biSizeImage;  //size of image in bytes
    UINT32 biXPelsPerMeter;  // number of pixels per meter in x axis
    UINT32 biYPelsPerMeter;  // number of pixels per meter in y axis
    UINT32 biClrUsed;  // number of colors used by the bitmap
    UINT32 biClrImportant;  // number of colors that are important
};
#pragma pack(pop)


_SMIL RES_T readBMP(const char* filename, Image<UINT8> *image);
_SMIL RES_T writeBMP(Image<UINT8> *image, const char *filename);

#endif // _D_IMAGE_IO_BMP_H