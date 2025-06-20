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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _D_COLOR_H
#define _D_COLOR_H

#include "Core/include/private/DMultichannelTypes.hpp"

#ifdef RGB
#undef RGB
#endif

namespace smil {

  using COLOR_UINT8_3 = MultichannelType<UINT8, 3>;
  using COLOR_UINT8_3_Array = MultichannelArray<UINT8, 3>;

  template <>
  struct ImDtTypes<COLOR_UINT8_3> {
    using pixelType = COLOR_UINT8_3;
    using lineType = COLOR_UINT8_3_Array;
    using restrictLineType = COLOR_UINT8_3_Array;
    using sliceType = lineType *;
    using volType = sliceType *;

    using floatType = MultichannelType<double, 3>;

    static inline pixelType min() {
      return {};
    }
    static inline pixelType max() {
      return {255};
    }
    static inline size_t cardinal() {
      return 256 * 256 * 256;
    }
    static inline lineType createLine(size_t lineLen) {
      lineType arr;
      arr.createArrays(lineLen);
      return arr;
    }
    static inline void deleteLine(lineType arr) {
      arr.deleteArrays();
    }
    static inline unsigned long ptrOffset(lineType p,
                                          unsigned long n = SIMD_VEC_SIZE) {
      return (size_t(p.arrays[0])) & (n - 1);
    }
    static inline std::string toString(const COLOR_UINT8_3 &val) {
      stringstream str;
      str << "(";
      for(UINT i = 0; i < 2; i++)
        str << double(val[i]) << ",";
      str << double(val[2]) << ")";
      return str.str();
    }
  };

  using RGBArray = COLOR_UINT8_3_Array;

  struct RGB
#ifndef SWIG
    : public COLOR_UINT8_3
#endif // SWIG
  {
#ifndef SWIG
    UINT8 &r;
    UINT8 &g;
    UINT8 &b;
#else
    UINT8 r;
    UINT8 g;
    UINT8 b;
#endif // SWIG
    RGB()
      : MultichannelType<UINT8, 3>(0), r(c[0]), g(c[1]), b(c[2])

    {
    }
    RGB(const UINT &val)
      : MultichannelType<UINT8, 3>(val), r(c[0]), g(c[1]), b(c[2]) {
    }
    RGB(int _r, int _g, int _b)
      : MultichannelType<UINT8, 3>(_r, _g, _b), r(c[0]), g(c[1]), b(c[2]) {
    }
    RGB(const COLOR_UINT8_3 &rhs)
      : MultichannelType<UINT8, 3>(rhs), r(c[0]), g(c[1]), b(c[2]) {
    }
    RGB(const RGB &rhs)
      : MultichannelType<UINT8, 3>(rhs), r(c[0]), g(c[1]), b(c[2]) {
    }
    ~RGB() override = default;
    RGB &operator=(const RGB &rhs) {
      for(UINT i = 0; i < 3; i++)
        c[i] = rhs.value(i);
      return *this;
    }
    void printSelf(ostream &os = std::cout, string = "") const {
      os << "(" << double(c[0]) << "," << double(c[1]) << "," << double(c[2])
         << ")";
    }
  };

  template <>
  struct ImDtTypes<RGB> : public ImDtTypes<COLOR_UINT8_3> {
    using pixelType = RGB;
    using lineType = RGBArray;
    static inline pixelType min() {
      return {};
    }
    static inline pixelType max() {
      return {255};
    }
    static inline size_t cardinal() {
      return 256 * 256 * 256;
    }
  };

  template <>
  inline const char *getDataTypeAsString(RGB *) {
    return "RGB";
  }

  using COLOR_32 = MultichannelType<UINT8, 4>;
  using COLOR_32_Array = MultichannelArray<UINT8, 4>;

  template <>
  struct ImDtTypes<COLOR_32> {
    using pixelType = COLOR_32;
    using lineType = COLOR_32_Array;
    using sliceType = lineType *;
    using volType = sliceType *;

    static inline pixelType min() {
      return {};
    }
    static inline pixelType max() {
      return {255};
    }
    static inline lineType createLine(size_t lineLen) {
      lineType arr;
      arr.createArrays(lineLen);
      return arr;
    }
    static inline void deleteLine(lineType arr) {
      arr.deleteArrays();
    }
    static inline unsigned long ptrOffset(lineType p,
                                          unsigned long n = SIMD_VEC_SIZE) {
      return (size_t(p.arrays[0])) & (n - 1);
    }
    static inline std::string toString(const COLOR_32 &val) {
      stringstream str;
      for(UINT i = 0; i < 3; i++)
        str << double(val[i]) << ", ";
      str << double(val[3]);
      return str.str();
    }
  };

} // namespace smil

#endif // _D_COLOR_H
