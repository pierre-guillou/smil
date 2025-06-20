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

#ifndef _DRGB_H
#define _DRGB_H

#include "Core/include/DColor.h"
#include "DImage_RGB.h"
#include "DLineArith_RGB.h"

#ifdef USE_QT

#include "Gui/Qt/DQtImageViewer.hpp"
#include "Gui/Qt/DQtImageViewer.hxx"

namespace smil {
  template <>
  inline void QtImageViewer<RGB>::setImage(Image<RGB> &im) {
    BASE_QT_VIEWER::imageFormat = QImage::Format_ARGB32_Premultiplied;
    ImageViewer<RGB>::setImage(im);
    BASE_QT_VIEWER::setImageSize(im.getWidth(), im.getHeight(), im.getDepth());
    if(im.getName() != string(""))
      setName(this->image->getName());
  }

  template <>
  inline void QtImageViewer<RGB>::drawImage() {
    Image<RGB>::sliceType lines = this->image->getSlices()[slider->value()];
    Image<RGB>::lineType pixels;
    using arrayType = Image<UINT8>::lineType;

    size_t w = this->image->getWidth();
    size_t h = this->image->getHeight();

    QRgb *destLine;
    arrayType rArray, gArray, bArray;

    for(size_t j = 0; j < h; j++) {
      destLine = (QRgb *)(this->qImage->scanLine(j));
      pixels = *lines++;
      rArray = pixels.arrays[0];
      gArray = pixels.arrays[1];
      bArray = pixels.arrays[2];
      for(size_t i = 0; i < w; i++) {
        destLine[i] = qRgb(rArray[i], gArray[i], bArray[i]);
      }
    }
  }

  template <>
  inline void
    QtImageViewer<RGB>::displayPixelValue(size_t x, size_t y, size_t z) {
    RGB pixVal;

    pixVal = this->image->getPixel(x, y, z);
    QString txt = "(" + QString::number(x) + ", " + QString::number(y);
    if(this->image->getDepth() > 1)
      txt = txt + ", " + QString::number(z);
    txt = txt + ") " + QString::number(pixVal[0]) + ","
          + QString::number(pixVal[1]) + "," + QString::number(pixVal[2]);
    valueLabel->setText(txt);
    valueLabel->adjustSize();
  }

  template <>
  inline void QtImageViewer<RGB>::drawOverlay(const Image<RGB> & /*im*/) {
  }

#ifdef USE_QWT
  template <>
  inline void QtImageViewer<RGB>::displayHistogram(bool /*update*/) {
  }
  template <>
  inline void QtImageViewer<RGB>::displayProfile(bool /*update*/) {
  }
#endif // USE_QWT
} // namespace smil

#endif // USE_QT

namespace smil {
#if defined SWIGPYTHON && defined USE_NUMPY
  template <>
  PyObject *Image<RGB>::getNumpyArray(bool /*c_contigous*/) {
    return NULL;
  }
#endif // defined SWIGPYTHON and defined USE_NUMPY

} // namespace smil

#endif // _DRGB_H
