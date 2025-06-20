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

#ifndef _MORPH_IMAGE_OPERATIONS_HXX
#define _MORPH_IMAGE_OPERATIONS_HXX

#include "DMorphImageOperations.hpp"

#ifdef USE_OPEN_MP
#include <omp.h>
#endif // USE_OPEN_MP

namespace smil {
  template <class T_in, class T_out>
  RES_T MorphImageFunctionBase<T_in, T_out>::initialize(const imageInType &imIn,
                                                        imageOutType &imOut,
                                                        const StrElt &se) {
    this->imageIn = &imIn;
    this->imageOut = &imOut;

    imIn.getSize(imSize);
    slicesIn = imIn.getSlices();
    slicesOut = imOut.getSlices();
    pixelsIn = imIn.getPixels();
    pixelsOut = imOut.getPixels();

    sePoints = se.points;

    oddSe = se.odd;

    sePointNbr = sePoints.size();
    relativeOffsets.clear();
    vector<IntPoint>::iterator pt = sePoints.begin();
    se_xmin = ImDtTypes<int>::max();
    se_xmax = ImDtTypes<int>::min();
    se_ymin = ImDtTypes<int>::max();
    se_ymax = ImDtTypes<int>::min();
    se_zmin = ImDtTypes<int>::max();
    se_zmax = ImDtTypes<int>::min();
    while(pt != sePoints.end()) {
      if(pt->x < se_xmin)
        se_xmin = pt->x;
      if(pt->x > se_xmax)
        se_xmax = pt->x;
      if(pt->y < se_ymin)
        se_ymin = pt->y;
      if(pt->y > se_ymax)
        se_ymax = pt->y;
      if(pt->z < se_zmin)
        se_zmin = pt->z;
      if(pt->z > se_zmax)
        se_zmax = pt->z;

      relativeOffsets.push_back(pt->x + pt->y * imSize[0]
                                + pt->z * imSize[0] * imSize[1]);
      pt++;
    }
    return RES_OK;
  }

  template <class T_in, class T_out>
  RES_T
    MorphImageFunctionBase<T_in, T_out>::finalize(const imageInType & /*imIn*/,
                                                  imageOutType & /*imOut*/,
                                                  const StrElt & /*se*/) {
    return RES_OK;
  }

  template <class T_in, class T_out>
  RES_T MorphImageFunctionBase<T_in, T_out>::_exec(const imageInType &imIn,
                                                   imageOutType &imOut,
                                                   const StrElt &se) {
    ASSERT_ALLOCATED(&imIn)
    ASSERT_SAME_SIZE(&imIn, &imOut)

    if((void *)&imIn == (void *)&imOut) {
      Image<T_in> tmpIm(imIn, true); // clone
      return _exec(imIn, imOut, se);
    }

    ImageFreezer freeze(imOut);

    StrElt se2;
    if(se.size > 1)
      se2 = se.homothety(se.size);
    else
      se2 = se;

    initialize(imIn, imOut, se2);

    RES_T retVal;

    retVal = processImage(imIn, imOut, se2);

    finalize(imIn, imOut, se);

    return retVal;
  }
  template <class T_in, class T_out>
  RES_T MorphImageFunctionBase<T_in, T_out>::_exec(const imageInType &imIn,
                                                   const StrElt &se) {
    ASSERT_ALLOCATED(&imIn)

    imageOutType _imOut(imIn);

    StrElt se2;
    if(se.size > 1)
      se2 = se.homothety(se.size);
    else
      se2 = se;

    this->initialize(imIn, _imOut, se2);

    RES_T retVal;

    retVal = processImage(imIn, _imOut, se2);

    finalize(imIn, _imOut, se);

    return retVal;
  }

  template <class T_in, class T_out>
  RES_T MorphImageFunctionBase<T_in, T_out>::processImage(
    const imageInType & /*imIn*/, imageOutType & /*imOut*/, const StrElt &se) {
    for(size_t curSlice = 0; curSlice < imSize[2]; curSlice++) {
      processSlice(*slicesIn, *slicesOut, imSize[1], se);
      slicesIn++;
      slicesOut++;
    }
    return RES_OK;
  }
  //     virtual RES_T processImage(imageInType &imIn, imageOutType &imOut, hSE
  //     &se)
  //     {
  //     }

  template <class T_in, class T_out>
  void MorphImageFunctionBase<T_in, T_out>::processSlice(sliceInType linesIn,
                                                         sliceOutType linesOut,
                                                         size_t &lineNbr,
                                                         const StrElt &se) {
    for(size_t curLine = 0; curLine < lineNbr; curLine++)
      processLine(linesIn[curLine], linesOut[curLine], imSize[0], se);
  }

  // TODO : offset list for 3D odd SE !!
  template <class T_in, class T_out>
  void MorphImageFunctionBase<T_in, T_out>::processLine(lineInType pixIn,
                                                        lineOutType /*pixOut*/,
                                                        size_t &pixNbr,
                                                        const StrElt &se) {
    int x, y, z;
    IntPoint p;
    size_t offset = pixIn - pixelsIn;
    vector<IntPoint> ptList;
    vector<int> relOffsetList;
    vector<int> offsetList;

    size_t curSlice = offset / (imSize[0] * imSize[1]);
    size_t curLine = (offset / imSize[0]) % imSize[1];
    size_t curPixel = 0;

    bool oddLine = se.odd && (curLine) % 2;
    //     int dx;

    // Remove points wich are outside the image
    for(UINT i = 0; i < sePointNbr; i++) {
      p = sePoints[i];
      y = curLine + p.y;
      z = curSlice + p.z;
      if(y >= 0 && y < int(imSize[1]) && z >= 0 && z < int(imSize[2])) {
        if(oddLine && ((y + 1) % 2) != 0)
          p.x += 1;
        ptList.push_back(p);
        if(oddLine && ((y + 1) % 2) != 0)
          relOffsetList.push_back(relativeOffsets[i] + 1);
        else
          relOffsetList.push_back(relativeOffsets[i]);
      }
    }
    UINT ptNbr = ptList.size();

    // Left border
    while((int)curPixel < -se_xmin) {
      offsetList.clear();
      for(UINT i = 0; i < ptNbr; i++) {
        x = curPixel + ptList[i].x;

        if(x >= 0 && x < (int)imSize[0])
          offsetList.push_back(relOffsetList[i]);
      }
      processPixel(offset, offsetList);
      curPixel++;
      offset++;
    }

    // Middle
    offsetList.clear();
    for(UINT i = 0; i < ptNbr; i++)
      offsetList.push_back(relOffsetList[i]);
    while(curPixel < pixNbr - se_xmax) {
      processPixel(offset, offsetList);
      curPixel++;
      offset++;
    }

    // Right border
    while(curPixel < pixNbr) {
      offsetList.clear();
      for(UINT i = 0; i < ptNbr; i++) {
        x = curPixel + ptList[i].x;

        if(x >= 0 && x < int(imSize[0]))
          offsetList.push_back(relOffsetList[i]);
      }
#ifdef USE_OPEN_MP
#pragma omp critical
#endif // USE_OPEN_MP
      {
        processPixel(offset, offsetList);
      }
      curPixel++;
      offset++;
    }
  }

  template <class T_in, class T_out>
  void
    MorphImageFunctionBase<T_in, T_out>::processPixel(size_t /*pointOffset*/,
                                                      vector<int> &dOffsets) {
    // Example: dilation function
    vector<int>::iterator it = dOffsets.begin();
    while(it != dOffsets.end()) {
      // pixelsOut[pointOffset] = max(pixelsOut[pointOffset],
      // pixelsIn[pointOffset + *dOffset]);
      it++;
    }
  }

#if 0
  template <class T_in, class T_out>
  void
  MorphImageFunctionBase<T_in, T_out>::processPixel(IntPoint &point,
                                                    vector<IntPoint> &dOffsets)
  {
    // Example: dilation function
    vector<int>::iterator it = dOffsets.begin();
    while (it != dOffsets.end()) {
      // pixelsOut[pointOffset] = max(pixelsOut[pointOffset], pixelsIn[pointOffset + *dOffset]);
      it++;
    }
  }
#endif

  ///******************************************************************************
  ///******************************************************************************
  ///******************************************************************************
  ///******************************************************************************

  template <class T_in, class lineFunction_T, class T_out, bool Enable>
  bool MorphImageFunction<T_in, lineFunction_T, T_out, Enable>::isInplaceSafe(
    const StrElt &se) {
    int st = se.getType();

    switch(st) {
      case SE_Horiz:
        return true;
      case SE_Vert:
        return true;
      case SE_Hex:
        return true;
      case SE_Squ:
        return true;
      case SE_Cube:
        return true;
      case SE_Cross:
        return true;
      default:
        return false;
    }
  }

  template <class T_in, class lineFunction_T, class T_out, bool Enable>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_out, Enable>::initialize(
    const imageInType &imIn, imageOutType & /*imOut*/, const StrElt & /*se*/) {
    this->lineLen = imIn.getWidth();

    this->borderBuf = ImDtTypes<T_in>::createLine(this->lineLen);
    this->cpBuf = ImDtTypes<T_in>::createLine(this->lineLen);
    fillLine<T_in> f;
    f(this->borderBuf, this->lineLen, this->borderValue);

    return RES_OK;
  }

  template <class T_in, class lineFunction_T, class T_out, bool Enable>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_out, Enable>::finalize(
    const imageInType & /*imIn*/,
    imageOutType & /*imOut*/,
    const StrElt & /*se*/) {
    ImDtTypes<T_in>::deleteLine(this->borderBuf);
    ImDtTypes<T_in>::deleteLine(this->cpBuf);

    return RES_OK;
  }

  template <class T_in, class lineFunction_T, class T_out, bool Enable>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_out, Enable>::_exec(
    const imageInType &imIn, imageOutType &imOut, const StrElt &se) {
    ASSERT_ALLOCATED(&imIn)
    ASSERT_SAME_SIZE(&imIn, &imOut)

    if((void *)&imIn == (void *)&imOut) {
      Image<T_in> tmpIm(imIn, true); // clone
      return _exec(imIn, imOut, se);
    }

    ImageFreezer freeze(imOut);

    StrElt se2;
    if(se.size > 1)
      se2 = se.homothety(se.size);
    else
      se2 = se;

    this->initialize(imIn, imOut, se2);

    RES_T retVal;

    retVal = _exec_single(imIn, imOut, se2);

    this->finalize(imIn, imOut, se);

    return retVal;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::_exec(
    const imageType &imIn, imageType &imOut, const StrElt &se) {
    ASSERT_ALLOCATED(&imIn, &imOut);

    int seSize = se.size;
    int seType = se.getType();

    if(seSize == 0) {
      copy(imIn, imOut);
      return RES_OK;
    }

    this->initialize(imIn, imOut, se);

    if(seType == SE_Rhombicuboctahedron) {
      _exec_rhombicuboctahedron(imIn, imOut, se.size);
      this->finalize(imIn, imOut, se);
      return RES_OK;
    }

    ImageFreezer freezer(imOut);

    Image<T_in> *inImage;
    Image<T_in> *outImage, *tmpImage = NULL;

    inImage = (Image<T_in> *)&imIn;

    if(this->isInplaceSafe(se) || (&imIn != &imOut && seSize == 1))
      outImage = (Image<T_in> *)&imOut;
    else
      outImage = tmpImage = new Image<T_in>(imOut);

    //     else
    {
      _exec_single(*inImage, *outImage, se);

      if(seSize > 1) {
        if(tmpImage)
          inImage = tmpImage;
        else
          inImage = &imOut;

        outImage = &imOut;

        for(int i = 1; i < seSize; i++) {
          _exec_single(*inImage, *outImage, se);
          if(i < seSize - 1)
            swap(inImage, outImage);
        }
      }
    }

    if(tmpImage) {
      if(outImage != &imOut)
        copy(*outImage, imOut);
      delete tmpImage;
    }

    this->finalize(imIn, imOut, se);

    imOut.modified();
    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::_exec_single(
    const imageType &imIn, imageType &imOut, const StrElt &se) {
    int st = se.getType();

    switch(st) {
      case SE_Hex:
        return _exec_single_hexagonal_SE(imIn, imOut);
      case SE_Squ:
        return _exec_single_square_SE(imIn, imOut);
      case SE_Horiz:
        return _exec_single_horizontal_segment(imIn, 1, imOut);
      case SE_Cross:
        return _exec_single_cross(imIn, imOut);
      case SE_Vert:
        return _exec_single_vertical_segment(imIn, imOut);
      case SE_Cube:
        return _exec_single_cube_SE(imIn, imOut);
        //           case SE_Cross3D:
        //             return _exec_single_cross_3d(imIn, imOut);
      default:
        return _exec_single_generic(imIn, imOut, se);
    }

    return RES_ERR_NOT_IMPLEMENTED;
  }

  template <class T_in, class lineFunction_T>
  RES_T
    MorphImageFunction<T_in, lineFunction_T, T_in, true>::_exec_single_generic(
      const imageType &imIn, imageType &imOut, const StrElt &se) {
    int sePtsNumber = se.points.size();
    if(sePtsNumber == 0)
      return RES_OK;

    int nSlices = imIn.getSliceCount();
    int nLines = imIn.getHeight();

    int nthreads = Core::getInstance()->getNumberOfThreads();
    lineType *_bufs = this->createAlignedBuffers(2 * nthreads, this->lineLen);
    lineType tmpBuf = _bufs[0];
    lineType tmpBuf2 = _bufs[nthreads];

    const Image<T_in> *tmpIm;

    if(&imIn == &imOut)
      tmpIm = new Image<T_in>(imIn, true); // clone
    else
      tmpIm = &imIn;

    // volType srcSlices = tmpIm->getSlices();
    volType destSlices = imOut.getSlices();

    // lineInType *srcLines;
    lineType *destLines, lineOut;

    bool oddSe = se.odd;
    int oddLine = 0;

    int l, p;
#ifdef USE_OPEN_MP
    int tid;
#endif // USE_OPEN_MP
    int x, y, z;
    vector<IntPoint> pts = se.points;

    for(int s = 0; s < nSlices; s++) {
      destLines = destSlices[s];

#ifdef USE_OPEN_MP
#pragma omp parallel private(tid, tmpBuf, tmpBuf2, x, y, z, lineOut, p) \
  firstprivate(pts, oddLine) num_threads(nthreads)
#endif // USE_OPEN_MP
      {
#ifdef USE_OPEN_MP
        tid = omp_get_thread_num();
        tmpBuf = _bufs[tid];
        tmpBuf2 = _bufs[tid + nthreads];
#endif // _OPENMP

#ifdef USE_OPEN_MP
#pragma omp for
#endif // USE_OPEN_MP
        for(l = 0; l < nLines; l++) {
          if(oddSe)
            oddLine = ((l + 1) % 2 && (s + 1) % 2);
          z = s - pts[0].z;
          y = l - pts[0].y;
          x = pts[0].x + (oddLine && y % 2);

          this->_extract_translated_line(tmpIm, x, y, z, tmpBuf);

          lineOut = destLines[l];
          for(p = 1; p < sePtsNumber; p++) {
            z = s - pts[p].z;
            y = l - pts[p].y;
            x = pts[p].x + (oddLine && y % 2);

            this->_extract_translated_line(tmpIm, x, y, z, tmpBuf2);
            this->lineFunction._exec(tmpBuf, tmpBuf2, this->lineLen, tmpBuf);
          }

          copyLine<T_in>(tmpBuf, this->lineLen, lineOut);
        }
      }
    }

    if(&imIn == &imOut)
      delete tmpIm;

    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_single_hexagonal_SE(const imageType &imIn, imageType &imOut) {
    int nSlices = imIn.getSliceCount();
    int nLines = imIn.getHeight();

    //     int nthreads = Core::getInstance()->getNumberOfThreads();
    typename ImDtTypes<T_in>::matrixType _bufs(
      5, typename ImDtTypes<T_in>::vectorType(this->lineLen));

    lineType buf0 = _bufs[0].data();
    lineType buf1 = _bufs[1].data();
    lineType buf2 = _bufs[2].data();
    lineType buf3 = _bufs[3].data();
    lineType buf4 = _bufs[4].data();

    lineType tmpBuf;

    volType srcSlices = imIn.getSlices();
    volType destSlices = imOut.getSlices();

    sliceType srcLines;
    sliceType destLines;

    lineType curSrcLine;
    lineType curDestLine;

    for(int s = 0; s < nSlices; s++) {
      srcLines = srcSlices[s];
      destLines = destSlices[s];

      // Process first line
      this->_exec_shifted_line(
        srcLines[0], srcLines[0], -1, this->lineLen, buf0, this->cpBuf);
      this->_exec_shifted_line(buf0, buf0, 1, this->lineLen, buf3, buf4);

      this->_exec_shifted_line(
        srcLines[1], srcLines[1], 1, this->lineLen, buf1, this->cpBuf);
      lineFunction(buf3, buf1, this->lineLen, buf4);
      lineFunction(this->borderBuf, buf4, this->lineLen, destLines[0]);

      //     int tid;
      // #pragma omp parallel
      {
        int l;

        // #pragma omp parallel for private(l) shared(tmpIm) ordered
        for(l = 2; l < nLines; l++) {
          curSrcLine = srcLines[l];
          curDestLine = destLines[l - 1];

          if(!((l % 2 == 0) ^ (s % 2 == 0))) {
            this->_exec_shifted_line(
              curSrcLine, curSrcLine, -1, this->lineLen, buf2, this->cpBuf);
            this->_exec_shifted_line(buf1, buf1, -1, this->lineLen, buf3, buf4);
          } else {
            this->_exec_shifted_line(
              curSrcLine, curSrcLine, 1, this->lineLen, buf2, this->cpBuf);
            this->_exec_shifted_line(buf1, buf1, 1, this->lineLen, buf3, buf4);
          }

          lineFunction(buf0, buf2, this->lineLen, buf4);
          lineFunction(buf3, buf4, this->lineLen, curDestLine);
          tmpBuf = buf0;
          buf0 = buf1;
          buf1 = buf2;
          buf2 = tmpBuf;
        }
      }

      if(!((nLines % 2 == 0) ^ (s % 2 == 0)))
        this->_exec_shifted_line(buf1, buf1, -1, this->lineLen, buf3, buf4);
      else
        this->_exec_shifted_line(buf1, buf1, 1, this->lineLen, buf3, buf4);
      lineFunction(buf3, buf0, this->lineLen, buf4);
      lineFunction(this->borderBuf, buf4, this->lineLen, destLines[nLines - 1]);
    }

    //     this->deleteAlignedBuffers();

    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_single_square_SE(const imageType &imIn, imageType &imOut) {
    _exec_single_vertical_segment(imIn, imOut);
    _exec_single_horizontal_segment(imOut, 1, imOut);

    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T
    MorphImageFunction<T_in, lineFunction_T, T_in, true>::_exec_single_cube_SE(
      const imageType &imIn, imageType &imOut) {
    _exec_single_vertical_segment(imIn, imOut);
    _exec_single_horizontal_segment(imOut, 1, imOut);
    _exec_single_depth_segment(imOut, 1, imOut);

    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_single_horizontal_2points(const imageType &imIn,
                                    int dx,
                                    imageType &imOut,
                                    bool) {
    int lineCount = imIn.getLineCount();

    int nthreads = Core::getInstance()->getNumberOfThreads();
    lineType *_bufs = this->createAlignedBuffers(nthreads, this->lineLen);
    lineType buf = _bufs[0];

    sliceType srcLines = imIn.getLines();
    sliceType destLines = imOut.getLines();

#ifdef USE_OPEN_MP
    int tid;
#endif // USE_OPEN_MP
    int l;

#ifdef USE_OPEN_MP
#pragma omp parallel private(tid, buf) num_threads(nthreads)
#endif // USE_OPEN_MP
    {
#ifdef USE_OPEN_MP
      tid = omp_get_thread_num();
      buf = _bufs[tid];
#pragma omp for
#endif
      for(l = 0; l < lineCount; l++) {
        // Todo: if oddLines...
        shiftLine<T_in>(srcLines[l], dx, this->lineLen, buf, this->borderValue);
        this->lineFunction(buf, srcLines[l], this->lineLen, destLines[l]);
      }
    }
    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_single_vertical_2points(const imageType &imIn,
                                  int dy,
                                  imageType &imOut) {
    int imHeight = imIn.getHeight();
    volType srcSlices = imIn.getSlices();
    volType destSlices = imOut.getSlices();
    sliceType srcLines;
    sliceType destLines;

    int l;

    for(size_t s = 0; s < imIn.getDepth(); s++) {
      srcLines = srcSlices[s];
      destLines = destSlices[s];

      if(dy > 0) {
        for(l = 0; l < imHeight - dy; l++)
          this->lineFunction(
            srcLines[l], srcLines[l + dy], this->lineLen, destLines[l]);
        for(l = imHeight - dy; l < imHeight; l++)
          this->lineFunction(
            srcLines[l], this->borderBuf, this->lineLen, destLines[l]);
      } else {
        for(l = imHeight - 1; l >= -dy; l--)
          this->lineFunction(
            srcLines[l], srcLines[l + dy], this->lineLen, destLines[l]);
        for(l = -dy - 1; l >= 0; l--)
          this->lineFunction(
            srcLines[l], this->borderBuf, this->lineLen, destLines[l]);
      }
    }
    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_single_horizontal_segment(const imageType &imIn,
                                    int xsize,
                                    imageType &imOut) {
    int lineCount = imIn.getLineCount();

    int nthreads = Core::getInstance()->getNumberOfThreads();
    lineType *_bufs = this->createAlignedBuffers(2 * nthreads, this->lineLen);
    lineType buf1 = _bufs[0];
    lineType buf2 = _bufs[nthreads];

    sliceType srcLines = imIn.getLines();
    sliceType destLines = imOut.getLines();

    lineType lineIn;

#ifdef USE_OPEN_MP
    int tid;
#endif // USE_OPEN_MP
    int l, dx = xsize;

#ifdef USE_OPEN_MP
#pragma omp parallel private(tid, buf1, buf2, lineIn) firstprivate(dx) \
  num_threads(nthreads)
#endif // USE_OPEN_MP
    {
#ifdef USE_OPEN_MP
      tid = omp_get_thread_num();
      buf1 = _bufs[tid];
      buf2 = _bufs[tid + nthreads];
#pragma omp for
#endif
      for(l = 0; l < lineCount; l++) {
        // Todo: if oddLines...
        lineIn = srcLines[l];
        shiftLine<T_in>(lineIn, dx, this->lineLen, buf1, this->borderValue);
        this->lineFunction(buf1, lineIn, this->lineLen, buf2);
        shiftLine<T_in>(lineIn, -dx, this->lineLen, buf1, this->borderValue);
        this->lineFunction(buf1, buf2, this->lineLen, destLines[l]);
      }
    }

    return RES_OK;
  }

  // Z-Horizontal segment
  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_single_depth_segment(const imageType &imIn,
                               int /*zsize*/,
                               imageType &imOut) {
    size_t w, h, d;
    imIn.getSize(&w, &h, &d);

    int nthreads = Core::getInstance()->getNumberOfThreads();
    lineType *_bufs = this->createAlignedBuffers(2 * nthreads, this->lineLen);
    lineType buf1 = _bufs[0];
    lineType buf2 = _bufs[nthreads];

    volType srcSlices = imIn.getSlices();
    volType destSlices = imOut.getSlices();

#ifdef USE_OPEN_MP
    int tid;
#endif // USE_OPEN_MP
    size_t y;

#ifdef USE_OPEN_MP
#pragma omp parallel private(tid, buf1, buf2) num_threads(nthreads)
#endif // USE_OPEN_MP
    {
#ifdef USE_OPEN_MP
      tid = omp_get_thread_num();
      buf1 = _bufs[tid];
      buf2 = _bufs[tid + nthreads];
#pragma omp for
#endif
      for(y = 0; y < h; y++) {
        this->lineFunction(
          this->borderBuf, srcSlices[0][y], this->lineLen, buf1);

        for(size_t z = 1; z < d; z++) {
          // Todo: if oddLines...
          this->lineFunction(
            srcSlices[z][y], srcSlices[z - 1][y], this->lineLen, buf2);
          this->lineFunction(buf1, buf2, this->lineLen, destSlices[z - 1][y]);

          swap(buf1, buf2);
        }

        this->lineFunction(
          this->borderBuf, buf1, this->lineLen, destSlices[d - 1][y]);
      }
    }

    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_single_vertical_segment(const imageType &imIn, imageType &imOut) {
    UINT imHeight = imIn.getHeight();
    size_t imWidth = imIn.getWidth();
    volType srcSlices = imIn.getSlices();
    volType destSlices = imOut.getSlices();
    sliceType srcLines;
    sliceType destLines;

    int tid = 0,
        nthreads = MIN(Core::getInstance()->getNumberOfThreads(), imHeight / 4);
    nthreads = MAX(nthreads, 1);
    int nbufs = 4;
    lineType *_bufs
      = this->createAlignedBuffers(nbufs * nthreads, this->lineLen);
    lineType buf1, buf2, firstLineBuf;

    size_t firstLine, blockSize;

    for(size_t s = 0; s < imIn.getDepth(); s++) {
      srcLines = srcSlices[s];
      destLines = destSlices[s];

#ifdef USE_OPEN_MP
#pragma omp parallel private( \
    tid, blockSize, firstLine, buf1, buf2, firstLineBuf) num_threads(nthreads)
#endif
      {
#ifdef USE_OPEN_MP
        tid = omp_get_thread_num();
#endif
        buf1 = _bufs[tid * nbufs];
        buf2 = _bufs[tid * nbufs + 1];
        firstLineBuf = _bufs[tid * nbufs + 2];

        blockSize = imHeight / nthreads;
        firstLine = tid * blockSize;
        if(tid == nthreads - 1)
          blockSize = imHeight - blockSize * tid;

        // Process first line
        copyLine<T_in>(srcLines[firstLine], imWidth, buf1);
        if(firstLine == 0)
          lineFunction(buf1, this->borderBuf, imWidth, buf2);
        else
          lineFunction(buf1, srcLines[firstLine - 1], imWidth, buf2);
        lineFunction(
          srcLines[firstLine], srcLines[firstLine + 1], imWidth, buf1);
        lineFunction(buf1, buf2, imWidth, firstLineBuf);

#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP

        for(size_t i = firstLine + 1; i < firstLine + blockSize - 1; i++) {
          lineFunction(srcLines[i], srcLines[i + 1], imWidth, buf2);
          lineFunction(buf1, buf2, imWidth, destLines[i]);

          swap(buf1, buf2);
        }

        if(firstLine + blockSize == imHeight)
          lineFunction(srcLines[firstLine + blockSize - 1], this->borderBuf,
                       imWidth, buf2);
        else
          lineFunction(srcLines[firstLine + blockSize - 1],
                       srcLines[firstLine + blockSize], imWidth, buf2);
        lineFunction(buf1, buf2, imWidth, destLines[firstLine + blockSize - 1]);

#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP

        // finaly write the first line
        copyLine<T_in>(firstLineBuf, imWidth, destLines[firstLine]);

      } // #pragma omp parallel
    }
    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T
    MorphImageFunction<T_in, lineFunction_T, T_in, true>::_exec_single_cross(
      const imageType &imIn, imageType &imOut) {
    UINT imHeight = imIn.getHeight();
    size_t imWidth = imIn.getWidth();
    volType srcSlices = imIn.getSlices();
    volType destSlices = imOut.getSlices();
    sliceType srcLines;
    sliceType destLines;

    int tid = 0,
        nthreads = MIN(Core::getInstance()->getNumberOfThreads(), imHeight / 4);
    nthreads = MAX(nthreads, 1);
    int nbufs = 6;
    lineType *_bufs
      = this->createAlignedBuffers(nbufs * nthreads, this->lineLen);
    lineType buf1, buf2, buf3, buf4, tmpBuf, firstLineBuf;
    lineType swap_buf;

    size_t firstLine, blockSize;

    for(size_t s = 0; s < imIn.getDepth(); s++) {
      srcLines = srcSlices[s];
      destLines = destSlices[s];

#ifdef USE_OPEN_MP
#pragma omp parallel private(tid, blockSize, firstLine, buf1, buf2, buf3, \
                               buf4, tmpBuf, firstLineBuf, swap_buf)      \
  num_threads(nthreads)
#endif
      {
#ifdef USE_OPEN_MP
        tid = omp_get_thread_num();
#endif
        buf1 = _bufs[tid * nbufs];
        buf2 = _bufs[tid * nbufs + 1];
        buf3 = _bufs[tid * nbufs + 2];
        buf4 = _bufs[tid * nbufs + 3];
        tmpBuf = _bufs[tid * nbufs + 4];
        firstLineBuf = _bufs[tid * nbufs + 5];

        blockSize = imHeight / nthreads;
        firstLine = tid * blockSize;
        if(tid == nthreads - 1)
          blockSize = imHeight - blockSize * tid;

        // Process first line
        copyLine<T_in>(srcLines[firstLine], imWidth, buf1);
        this->_exec_shifted_line_2ways(buf1, 1, imWidth, buf4, tmpBuf);

        copyLine<T_in>(srcLines[firstLine + 1], imWidth, buf2);

        lineFunction(buf4, buf2, imWidth, tmpBuf);
        if(firstLine == 0)
          lineFunction(this->borderBuf, tmpBuf, imWidth, firstLineBuf);
        else
          lineFunction(srcLines[firstLine - 1], tmpBuf, imWidth, firstLineBuf);

#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP

        for(size_t i = firstLine + 2; i < firstLine + blockSize; i++) {
          copyLine<T_in>(srcLines[i], imWidth, buf3);
          this->_exec_shifted_line_2ways(buf2, 1, imWidth, buf4, tmpBuf);

          lineFunction(buf1, buf3, imWidth, tmpBuf);
          lineFunction(buf4, tmpBuf, imWidth, destLines[i - 1]);

          swap_buf = buf1;
          buf1 = buf2;
          buf2 = buf3;
          buf3 = swap_buf;
        }

        this->_exec_shifted_line_2ways(buf2, 1, imWidth, buf4, tmpBuf);
        lineFunction(buf1, buf4, imWidth, buf4);
        if(firstLine + blockSize == imHeight)
          lineFunction(buf4, this->borderBuf, imWidth,
                       destLines[firstLine + blockSize - 1]);
        else
          lineFunction(buf4, srcLines[firstLine + blockSize], imWidth,
                       destLines[firstLine + blockSize - 1]);

#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP

        // finaly write the first line
        copyLine<T_in>(firstLineBuf, imWidth, destLines[firstLine]);

      } // #pragma omp parallel
    }
    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T
    MorphImageFunction<T_in, lineFunction_T, T_in, true>::_exec_single_cross_3d(
      const imageType &imIn, imageType &imOut) {
    size_t w, h, d;
    imIn.getSize(&w, &h, &d);

    volType srcSlices = imIn.getSlices();
    volType destSlices = imOut.getSlices();
    sliceType srcLines;
    sliceType destLines;

    int tid = 0,
        nthreads = MIN(Core::getInstance()->getNumberOfThreads(), h / 4);
    nthreads = MAX(nthreads, 1);
    int nbufs = 6;
    lineType *_bufs
      = this->createAlignedBuffers(nbufs * nthreads, this->lineLen);
    lineType buf1, buf2, buf3, buf4, tmp1, firstLineBuf;
    lineType swap_buf;

    size_t firstLine, blockSize;

    srcLines = srcSlices[0];
    destLines = destSlices[0];
#ifdef USE_OPEN_MP
#pragma omp parallel private(tid, blockSize, firstLine, buf1, buf2, buf3, \
                               buf4, tmp1, firstLineBuf, swap_buf)        \
  num_threads(nthreads)
#endif // USE_OPEN_MP
    {
#ifdef USE_OPEN_MP
      tid = omp_get_thread_num();
#endif
      buf1 = _bufs[tid * nbufs];
      buf2 = _bufs[tid * nbufs + 1];
      buf3 = _bufs[tid * nbufs + 2];
      buf4 = _bufs[tid * nbufs + 3];
      tmp1 = _bufs[tid * nbufs + 4];
      firstLineBuf = _bufs[tid * nbufs + 5];

      blockSize = h / nthreads;
      firstLine = tid * blockSize;
      if(tid == nthreads - 1)
        blockSize = h - blockSize * tid;

      // Process first line.
      copyLine<T_in>(srcLines[firstLine], w, buf1);
      this->_exec_shifted_line_2ways(buf1, 1, w, tmp1, buf4);
      lineFunction(this->borderBuf, srcSlices[0][firstLine], w, buf4);
      lineFunction(buf4, srcSlices[1][firstLine], w, buf4);
      lineFunction(buf4, tmp1, w, buf4);
      copyLine<T_in>(srcLines[firstLine + 1], w, buf2);
      lineFunction(buf4, buf2, w, tmp1);
      if(firstLine == 0)
        lineFunction(this->borderBuf, tmp1, w, firstLineBuf);
      else
        lineFunction(srcLines[firstLine - 1], tmp1, w, firstLineBuf);
#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP
      for(size_t i = firstLine + 2; i < firstLine + blockSize; ++i) {
        copyLine<T_in>(srcLines[i], w, buf3);
        this->_exec_shifted_line_2ways(buf2, 1, w, buf4, tmp1);
        lineFunction(buf1, buf3, w, tmp1);
        lineFunction(buf4, tmp1, w, tmp1);
        lineFunction(this->borderBuf, srcSlices[0][i - 1], w, buf4);
        lineFunction(buf4, srcSlices[1][i - 1], w, buf4);
        lineFunction(buf4, tmp1, w, destLines[i - 1]);

        swap_buf = buf1;
        buf1 = buf2;
        buf2 = buf3;
        buf3 = swap_buf;
      }

      this->_exec_shifted_line_2ways(buf2, 1, w, buf4, tmp1);
      lineFunction(buf1, buf4, w, tmp1);
      lineFunction(
        this->borderBuf, srcSlices[0][firstLine + blockSize - 1], w, buf4);
      lineFunction(buf4, srcSlices[1][firstLine + blockSize - 1], w, buf4);
      lineFunction(tmp1, buf4, w, buf4);
      if(firstLine + blockSize == h)
        lineFunction(
          buf4, this->borderBuf, w, destLines[firstLine + blockSize - 1]);
      else
        lineFunction(buf4, srcLines[firstLine + blockSize], w,
                     destLines[firstLine + blockSize - 1]);

#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP
      // finally write the first line
      copyLine<T_in>(firstLineBuf, w, destLines[firstLine]);

      for(size_t s = 1; s < d - 1; ++s) {
        srcLines = srcSlices[s];
        destLines = destSlices[s];

        blockSize = h / nthreads;
        firstLine = tid * blockSize;
        if(tid == nthreads - 1)
          blockSize = h - blockSize * tid;

        // Process first line.
        copyLine<T_in>(srcLines[firstLine], w, buf1);
        this->_exec_shifted_line_2ways(buf1, 1, w, tmp1, buf4);
        lineFunction(
          srcSlices[s - 1][firstLine], srcSlices[s][firstLine], w, buf4);
        lineFunction(buf4, srcSlices[s + 1][firstLine], w, buf4);
        lineFunction(buf4, tmp1, w, buf4);
        copyLine<T_in>(srcLines[firstLine + 1], w, buf2);
        lineFunction(buf4, buf2, w, tmp1);
        if(firstLine == 0)
          lineFunction(this->borderBuf, tmp1, w, firstLineBuf);
        else
          lineFunction(srcLines[firstLine - 1], tmp1, w, firstLineBuf);
#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP
        for(size_t i = firstLine + 2; i < firstLine + blockSize; ++i) {
          copyLine<T_in>(srcLines[i], w, buf3);
          this->_exec_shifted_line_2ways(buf2, 1, w, buf4, tmp1);
          lineFunction(buf1, buf3, w, tmp1);
          lineFunction(buf4, tmp1, w, tmp1);
          lineFunction(srcSlices[s - 1][i - 1], srcSlices[s][i - 1], w, buf4);
          lineFunction(buf4, srcSlices[s + 1][i - 1], w, buf4);
          lineFunction(buf4, tmp1, w, destLines[i - 1]);

          swap_buf = buf1;
          buf1 = buf2;
          buf2 = buf3;
          buf3 = swap_buf;
        }

        this->_exec_shifted_line_2ways(buf2, 1, w, buf4, tmp1);
        lineFunction(buf1, buf4, w, tmp1);
        lineFunction(srcSlices[s - 1][firstLine + blockSize - 1],
                     srcSlices[s][firstLine + blockSize - 1], w, buf4);
        lineFunction(
          buf4, srcSlices[s + 1][firstLine + blockSize - 1], w, buf4);
        lineFunction(tmp1, buf4, w, buf4);
        if(firstLine + blockSize == h)
          lineFunction(
            buf4, this->borderBuf, w, destLines[firstLine + blockSize - 1]);
        else
          lineFunction(buf4, srcLines[firstLine + blockSize], w,
                       destLines[firstLine + blockSize - 1]);

#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP
       // finally write the first line
        copyLine<T_in>(firstLineBuf, w, destLines[firstLine]);
      }

      srcLines = srcSlices[d - 1];
      destLines = destSlices[d - 1];

      blockSize = h / nthreads;
      firstLine = tid * blockSize;
      if(tid == nthreads - 1)
        blockSize = h - blockSize * tid;

      // Process first line.
      copyLine<T_in>(srcLines[firstLine], w, buf1);
      this->_exec_shifted_line_2ways(buf1, 1, w, tmp1, buf4);
      lineFunction(
        srcSlices[d - 2][firstLine], srcSlices[d - 1][firstLine], w, buf4);
      lineFunction(buf4, this->borderBuf, w, buf4);
      lineFunction(buf4, tmp1, w, buf4);
      copyLine<T_in>(srcLines[firstLine + 1], w, buf2);
      lineFunction(buf4, buf2, w, tmp1);
      if(firstLine == 0)
        lineFunction(this->borderBuf, tmp1, w, firstLineBuf);
      else
        lineFunction(srcLines[firstLine - 1], tmp1, w, firstLineBuf);
#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP
      for(size_t i = firstLine + 2; i < firstLine + blockSize; ++i) {
        copyLine<T_in>(srcLines[i], w, buf3);
        this->_exec_shifted_line_2ways(buf2, 1, w, buf4, tmp1);
        lineFunction(buf1, buf3, w, tmp1);
        lineFunction(buf4, tmp1, w, tmp1);
        lineFunction(srcSlices[d - 2][i - 1], srcSlices[d - 1][i - 1], w, buf4);
        lineFunction(buf4, this->borderBuf, w, buf4);
        lineFunction(buf4, tmp1, w, destLines[i - 1]);

        swap_buf = buf1;
        buf1 = buf2;
        buf2 = buf3;
        buf3 = swap_buf;
      }

      this->_exec_shifted_line_2ways(buf2, 1, w, buf4, tmp1);
      lineFunction(buf1, buf4, w, tmp1);
      lineFunction(srcSlices[d - 2][firstLine + blockSize - 1],
                   srcSlices[d - 1][firstLine + blockSize - 1], w, buf4);
      lineFunction(buf4, this->borderBuf, w, buf4);
      lineFunction(tmp1, buf4, w, buf4);
      if(firstLine + blockSize == h)
        lineFunction(
          buf4, this->borderBuf, w, destLines[firstLine + blockSize - 1]);
      else
        lineFunction(buf4, srcLines[firstLine + blockSize], w,
                     destLines[firstLine + blockSize - 1]);

#ifdef USE_OPEN_MP
#pragma omp barrier
#endif // USE_OPEN_MP
      // finally write the first line
      copyLine<T_in>(firstLineBuf, w, destLines[firstLine]);

    } // #pragma omp parallel

    return RES_OK;
  }

  template <class T_in, class lineFunction_T>
  RES_T MorphImageFunction<T_in, lineFunction_T, T_in, true>::
    _exec_rhombicuboctahedron(const imageType &imIn,
                              imageType &imOut,
                              unsigned int size) {
    double nbSquareDbl = (((double)size) / (1 + std::sqrt(2.)));
    double nbSquareFloor = floor(nbSquareDbl);
    int nbSquare
      = (int)(((nbSquareDbl - nbSquareFloor) < 0.5f) ? (nbSquareFloor)
                                                     : (nbSquareFloor + 1));

    ASSERT(_exec_single(imIn, imOut, Cross3DSE()) == RES_OK);

    for(size_t i = 1; i < size - nbSquare; ++i)
      ASSERT(_exec_single(imOut, imOut, Cross3DSE()) == RES_OK);

    for(int i = 0; i < nbSquare; ++i)
      ASSERT(_exec_single(imOut, imOut, CubeSE()) == RES_OK);

    return RES_OK;
  }

} // namespace smil

#endif // _MORPH_IMAGE_OPERATIONS_HXX
