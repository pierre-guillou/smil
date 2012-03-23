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

#ifdef SMIL_WRAP_Bit


#include "DImage_Bit.h"


template <>
void Image<Bit>::init() 
{ 
    slices = NULL;
    lines = NULL;
//     pixels = NULL;

    dataTypeSize = 1;
    
    allocatedSize = 0;
    
     viewer = NULL;
     name = NULL;
}

template <>
void* Image<Bit>::getVoidPointer(void) {
    return pixels.intArray;
}


template <>
RES_T Image<Bit>::restruct(void)
{
    if (slices)
	delete[] slices;
    if (lines)
	delete[] lines;
    
    lines =  new lineType[lineCount];
    slices = new sliceType[sliceCount];
    
    lineType *cur_array = lines;
    sliceType *cur_slice = slices;
    
    UINT intWidth = pixels.getIntWidth();
    UINT intNbrPerSlice = intWidth * height;
    BitArray::INT_TYPE *int0 = pixels.intArray;
    
    for (int k=0; k<(int)depth; k++, cur_slice++)
    {
      *cur_slice = cur_array;
      
      for (int j=0; j<(int)height; j++, cur_array++)
      {
	cur_array->setSize(width);
	cur_array->intArray = int0 + k*intNbrPerSlice + j*intWidth;
      }
    }
    
    return RES_OK;
}

template <>
RES_T Image<Bit>::allocate(void)
{
    if (allocated)
	return RES_ERR_BAD_ALLOCATION;
    
    pixels.setSize(width, height*depth);
    pixels.createIntArray();
    
    allocated = true;
    allocatedSize = pixels.getIntNbr()*BitArray::INT_TYPE_SIZE;
    
    restruct();
    
    return RES_OK;
}

template <>
RES_T Image<Bit>::deallocate(void)
{
    if (!allocated)
	return RES_OK;
    
    if (slices)
	delete[] slices;
    if (lines)
	delete[] lines;
    if (pixels.intArray)
	pixels.deleteIntArray();
    
    slices = NULL;
    lines = NULL;
//     pixels = NULL;

    allocated = false;
    allocatedSize = 0;
    
    return RES_OK;
}


template <>
Image<Bit>& Image<Bit>::clone(const Image<Bit> &rhs)
{ 
    bool isAlloc = rhs.isAllocated();
    setSize(rhs.getWidth(), rhs.getHeight(), rhs.getDepth(), isAlloc);
    if (isAlloc)
      memcpy(this->pixels.intArray, rhs.getPixels().intArray, allocatedSize*sizeof(CHAR_BIT));
    modified();
    return *this;
}



#endif // SMIL_WRAP_Bit
