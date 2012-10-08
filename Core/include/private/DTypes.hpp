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
 *     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
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


#ifndef _D_TYPES_HPP
#define _D_TYPES_HPP

#include <stdint.h>
#include <limits>
#include "DMemory.hpp"

using namespace std;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef uint64_t UINT64;

// typedef unsigned char __attribute__ ((vector_size (16))) alUINT8;

#ifndef _MSC_VER
typedef signed char INT8;
#endif // _MSC_VER
typedef short INT16;
typedef int INT32;


#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

template <class T>
struct ImDtTypes
{
    typedef T pixelType;
    typedef pixelType *lineType;
    typedef lineType *sliceType;
    typedef sliceType *volType;
    
    static inline pixelType min() { return numeric_limits<T>::min(); }
    static inline pixelType max() { return numeric_limits<T>::max(); }
    static inline lineType createLine(UINT lineLen) { return createAlignedBuffer<T>(lineLen); }
    static inline void deleteLine(lineType line) { deleteAlignedBuffer<T>(line); }
    static inline unsigned long ptrOffset(lineType p, unsigned long n=SIMD_VEC_SIZE) { return ((unsigned long)p) & (n-1); }
};



template <class T>
inline const char *getDataTypeAsString(T &val)
{
    return "Unknown";
}


#define DECL_DATA_TYPE_STR(_type) \
template <> \
inline const char *getDataTypeAsString(_type &val) { return #_type; } 

DECL_DATA_TYPE_STR(UINT8)
DECL_DATA_TYPE_STR(UINT16)
DECL_DATA_TYPE_STR(int)
DECL_DATA_TYPE_STR(float)
DECL_DATA_TYPE_STR(double)



#endif // _D_TYPES_HPP
