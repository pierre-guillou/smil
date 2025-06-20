/*
 * Copyright (c) 2011-2016, Matthieu FAESSEL and ARMINES
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _D_LINE_ARITH_RGB_H
#define _D_LINE_ARITH_RGB_H

#include "Base/include/private/DLineArith.hpp"
#include "Core/include/DColor.h"

/**
 * @ingroup Arith
 * @{
 */

namespace smil {
  template <>
  inline void copyLine<RGB>(const unaryLineFunctionBase<RGB>::lineInType lIn,
                            const size_t size,
                            unaryLineFunctionBase<RGB>::lineInType lOut) {
    for(UINT n = 0; n < 3; n++)
      memcpy(lOut.arrays[n], lIn.arrays[n], size * sizeof(UINT8));
  }

  template <class T1>
  RES_T copy(const Image<T1> & /*imIn*/,
             size_t /*startX*/,
             size_t /*startY*/,
             size_t /*startZ*/,
             size_t /*sizeX*/,
             size_t /*sizeY*/,
             size_t /*sizeZ*/,
             Image<RGB> & /*imOut*/,
             size_t /*outStartX*/ = 0,
             size_t /*outStartY*/ = 0,
             size_t /*outStartZ*/ = 0) {
    return RES_ERR;
  }

  template <>
  inline void shiftLine(const unaryLineFunctionBase<RGB>::lineInType lIn,
                        int dx,
                        size_t lineLen,
                        unaryLineFunctionBase<RGB>::lineInType lOut,
                        RGB borderValue) {
    for(UINT n = 0; n < 3; n++)
      shiftLine<UINT8>(
        lIn.arrays[n], dx, lineLen, lOut.arrays[n], borderValue[n]);
  }

  template <>
  struct fillLine<RGB> : public unaryLineFunctionBase<RGB> {
    using lineType = Image<RGB>::lineType;
    fillLine() = default;
    fillLine(const lineType lIn, const size_t size, const RGB value) {
      this->_exec(lIn, size, value);
    }

    void _exec(const lineType lIn, const size_t size, lineType lOut) override {
      copyLine<RGB>(lIn, size, lOut);
    }
    void _exec(lineType lInOut, const size_t size, const RGB value) override {
      for(UINT n = 0; n < 3; n++) {
        UINT8 *cArr = lInOut.arrays[n];
        UINT8 val = value[n];
        for(size_t i = 0; i < size; i++)
          cArr[i] = val;
      }
    }
  };

  template <>
  double vol(const Image<RGB> &imIn);

  //     template <>
  //     std::map<RGB, UINT> histogram(const Image<RGB> &imIn);

  template <>
  struct supLine<RGB> : public binaryLineFunctionBase<RGB> {
    using lineType = Image<RGB>::lineType;
    inline void _exec(const lineType lIn1,
                      const lineType lIn2,
                      const size_t size,
                      lineType lOut) override {
      for(UINT n = 0; n < 3; n++) {
        UINT8 *cArrIn1 = lIn1.arrays[n];
        UINT8 *cArrIn2 = lIn2.arrays[n];
        UINT8 *cArrOut = lOut.arrays[n];

        for(size_t i = 0; i < size; i++)
          cArrOut[i] = cArrIn1[i] > cArrIn2[i] ? cArrIn1[i] : cArrIn2[i];
      }
    }
  };

  template <>
  struct infLine<RGB> : public binaryLineFunctionBase<RGB> {
    using lineType = Image<RGB>::lineType;
    void _exec(const lineType lIn1,
               const lineType lIn2,
               const size_t size,
               lineType lOut) override {
      for(UINT n = 0; n < 3; n++) {
        UINT8 *cArrIn1 = lIn1.arrays[n];
        UINT8 *cArrIn2 = lIn2.arrays[n];
        UINT8 *cArrOut = lOut.arrays[n];

        for(size_t i = 0; i < size; i++)
          cArrOut[i] = cArrIn1[i] < cArrIn2[i] ? cArrIn1[i] : cArrIn2[i];
      }
    }
  };

  template <>
  struct equLine<RGB> : public binaryLineFunctionBase<RGB> {
    equLine() : trueVal(numeric_limits<UINT8>::max()), falseVal(0) {
    }

    UINT8 trueVal, falseVal;

    using lineType = Image<RGB>::lineType;
    void _exec(const lineType lIn1,
               const lineType lIn2,
               const size_t size,
               lineType lOut) override {
      for(UINT n = 0; n < 3; n++) {
        UINT8 *cArrIn1 = lIn1.arrays[n];
        UINT8 *cArrIn2 = lIn2.arrays[n];
        UINT8 *cArrOut = lOut.arrays[n];

        for(size_t i = 0; i < size; i++)
          cArrOut[i] = cArrIn1[i] == cArrIn2[i] ? trueVal : falseVal;
      }
    }
  };

} // namespace smil

/** @}*/

#endif // _D_LINE_ARITH_RGB_H
