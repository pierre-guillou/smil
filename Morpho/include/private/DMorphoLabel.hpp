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

#ifndef _D_MORPHO_LABEL_HPP
#define _D_MORPHO_LABEL_HPP

#include "Base/include/private/DImageArith.hpp"
#include "Core/include/DImage.h"
#include "DMorphImageOperations.hpp"
#include "Base/include/private/DBlobMeasures.hpp"

#include <set>
#include <map>
#include <functional>

namespace smil {
  /**
   * @ingroup Morpho
   * @defgroup Labelling Labelling
   *
   * @details The connected component @b labelling of a binary image is a
   * transformation directly associated with the notion of connectivity. It
   * consists in setting each pixel belonging to a connected component of the
   * input binary image to a specific grey level value, different values being
   * considered for each connected component (the background components are
   * usually not processed and they keep their original value, i.e., zero). The
   * resulting  image is called a <b>label image</b>.
   *
   * The notion of labelling extends directly to grey scale image. In this
   * latter case, a distinct label is given to each grey scale connected
   * component (flat zone) of the image.
   *
   * @see
   * - @SoilleBook{p. 35-38}
   * @{
   */

#ifndef SWIG

  /*
   *  ######  #    #  #    #   ####    #####   ####   #####    ####
   *  #       #    #  ##   #  #    #     #    #    #  #    #  #
   *  #####   #    #  # #  #  #          #    #    #  #    #   ####
   *  #       #    #  #  # #  #          #    #    #  #####        #
   *  #       #    #  #   ##  #    #     #    #    #  #   #   #    #
   *  #        ####   #    #   ####      #     ####   #    #   ####
   */
  /* @devdoc */
  /** @cond */
  template <class T1, class T2, class compOperatorT = std::equal_to<T1>>
  class labelFunctGeneric : public MorphImageFunctionBase<T1, T2> {
  public:
    typedef MorphImageFunctionBase<T1, T2> parentClass;
    typedef typename parentClass::imageInType imageInType;
    typedef typename parentClass::imageOutType imageOutType;

    size_t getLabelNbr() {
      return real_labels;
    }

    virtual RES_T initialize(const imageInType &imIn,
                             imageOutType &imOut,
                             const StrElt &se) {
      parentClass::initialize(imIn, imOut, se);
      fill(imOut, T2(0));
      labels = 0;
      real_labels = 0;
      max_value_label = ImDtTypes<T2>::max();
      return RES_OK;
    }

    virtual RES_T processImage(const imageInType &imIn,
                               imageOutType & /*imOut*/,
                               const StrElt & /*se*/) {
      this->pixelsIn = imIn.getPixels();

      size_t nbPixels = imIn.getPixelCount();
      for(size_t i = 0; i < nbPixels; i++) {
        if(this->pixelsOut[i] == T2(0)) {
          vector<int> dum;
          processPixel(i, dum);
        }
      }
      return RES_OK;
    }

    virtual void processPixel(size_t pointOffset,
                              SMIL_UNUSED vector<int> &dOffsets) {
      T1 pVal = this->pixelsIn[pointOffset];

      if(pVal == T1(0) || this->pixelsOut[pointOffset] != T2(0))
        return;

      queue<size_t> propagation;

      ++real_labels;
      ++labels;
      if(labels == max_value_label)
        labels = 1;
      this->pixelsOut[pointOffset] = (T2)labels;
      propagation.push(pointOffset);

      size_t pixPerLine = this->imSize[0];
      size_t pixPerSlice = this->imSize[0] * this->imSize[1];

      while(!propagation.empty()) {
        off_t x, y, z, n_x, n_y, n_z;

        bool oddLine = false;
        size_t curOffset, nbOffset;

        curOffset = propagation.front();
        pVal = this->pixelsIn[curOffset];

        z = curOffset / (pixPerSlice);
        y = (curOffset - z * pixPerSlice) / pixPerLine;
        x = curOffset - y * pixPerLine - z * pixPerSlice;

        oddLine = this->oddSe && (y % 2);

        for(UINT i = 0; i < this->sePointNbr; ++i) {
          IntPoint p = this->sePoints[i];

          n_z = z + p.z;
          if(n_z < 0 || n_z >= off_t(this->imSize[2]))
            continue;
          n_y = y + p.y;
          if(n_y < 0 || n_y >= off_t(this->imSize[1]))
            continue;
          n_x = x + p.x;
          if(oddLine && ((n_y + 1) % 2) != 0)
            n_x++;
          if(n_x < 0 || n_x >= off_t(this->imSize[0]))
            continue;

          // if (!this->imageIn->areCoordsInImage(n_x, n_y, n_z))
          //  continue;

          nbOffset = n_x + n_y * pixPerLine + n_z * pixPerSlice;
          if(nbOffset != curOffset && this->pixelsOut[nbOffset] != labels
             && compareFunc(this->pixelsIn[nbOffset], pVal)) {
            this->pixelsOut[nbOffset] = T2(labels);
            propagation.push(nbOffset);
          }
        }
        propagation.pop();
      }
    }

    compOperatorT compareFunc;

  protected:
    T2 labels;
    size_t real_labels;
    T2 max_value_label;
  };

  template <class T1, class T2, class compOperatorT = std::equal_to<T1>>
  class labelFunctFast : public MorphImageFunctionBase<T1, T2> {
  public:
    typedef MorphImageFunctionBase<T1, T2> parentClass;
    typedef typename parentClass::imageInType imageInType;
    typedef typename parentClass::imageOutType imageOutType;
    typedef typename imageInType::lineType lineInType;
    typedef typename imageInType::sliceType sliceInType;
    typedef typename imageOutType::lineType lineOutType;
    typedef typename imageOutType::sliceType sliceOutType;

    size_t getLabelNbr() {
      return labels_real;
    }

    virtual RES_T initialize(const imageInType &imIn,
                             imageOutType &imOut,
                             const StrElt &se) {
      parentClass::initialize(imIn, imOut, se);
      fill(imOut, T2(0));
      labels = T2(0);
      labels_real = 0;
      max_value_label = ImDtTypes<T2>::max();
      return RES_OK;
    }

    virtual RES_T processImage(const imageInType &imIn,
                               imageOutType &imOut,
                               const StrElt & /*se*/) {
      Image<T1> tmp(imIn);
      Image<T1> tmp2(imIn);
      ASSERT(clone(imIn, tmp) == RES_OK);
      if(this->imSize[2] == 1) {
        ASSERT(erode(tmp, tmp2, SquSE()) == RES_OK);
      } else {
        ASSERT(erode(tmp, tmp2, CubeSE()) == RES_OK);
      }
      ASSERT(sub(tmp, tmp2, tmp) == RES_OK);

      lineInType pixelsTmp = tmp.getPixels();

      // Adding the first point of each line to tmp.
#ifdef USE_OPEN_MP
#pragma omp parallel
#endif // USE_OPEN_MP
      {
#ifdef USE_OPEN_MP
#pragma omp for
#endif // USE_OPEN_MP
        for(size_t i = 0; i < this->imSize[2] * this->imSize[1]; ++i) {
          pixelsTmp[i * this->imSize[0]] = this->pixelsIn[i * this->imSize[0]];
        }
      }

      queue<size_t> propagation;
      int x, y, z, n_x, n_y, n_z;
      IntPoint p;

      T2 current_label = labels;
      bool is_not_a_gap = false;
      bool process_labeling = false;
      bool oddLine = 0;

      // First PASS to label the boundaries. //
      for(size_t i = 0; i < this->imSize[2] * this->imSize[1] * this->imSize[0];
          ++i) {
        if(i % (this->imSize[0]) == 0) {
          is_not_a_gap = false;
        }
        if(pixelsTmp[i] != T1(0)) {
          if(this->pixelsOut[i] == T2(0)) {
            if(!is_not_a_gap) {
              ++labels;
              ++labels_real;
              if(labels == max_value_label)
                labels = 1;
              current_label = (T2)labels;
            }
            this->pixelsOut[i] = current_label;
            process_labeling = true;
          } else {
            current_label = this->pixelsOut[i];
          }

          is_not_a_gap = true;
        }
        if(this->pixelsIn[i] == T1(0)) {
          is_not_a_gap = false;
        }

        if(process_labeling) {
          propagation.push(i);

          while(!propagation.empty()) {
            z = propagation.front() / (this->imSize[1] * this->imSize[0]);
            y = (propagation.front() - z * this->imSize[1] * this->imSize[0])
                / this->imSize[0];
            x = propagation.front() - y * this->imSize[0]
                - z * this->imSize[1] * this->imSize[0];

            oddLine = this->oddSe && (y % 2);
            size_t nbOffset;

            for(UINT i = 0; i < this->sePointNbr; ++i) {
              p = this->sePoints[i];
              n_x = x + p.x;
              n_y = y + p.y;
              n_x += (oddLine && ((n_y + 1) % 2) != 0);
              n_z = z + p.z;
              nbOffset = n_x + (n_y) * this->imSize[0]
                         + (n_z) * this->imSize[1] * this->imSize[0];
              if(n_x >= 0 && n_x < (int)this->imSize[0] && n_y >= 0
                 && n_y < (int)this->imSize[1] && n_z >= 0
                 && n_z < (int)this->imSize[2]
                 && compareFunc(
                   this->pixelsIn[nbOffset], pixelsTmp[propagation.front()])
                 && this->pixelsOut[nbOffset] != current_label) {
                this->pixelsOut[nbOffset] = current_label;
                propagation.push(nbOffset);
              }
            }

            propagation.pop();
          }
          process_labeling = false;
        }
      }
      // Propagate labels inside the borders //

      size_t nSlices = imIn.getDepth();
      size_t nLines = imIn.getHeight();
      size_t nPixels = imIn.getWidth();
      size_t l, v;
      T1 previous_value;
      T2 previous_label;

      sliceInType srcLines = imIn.getLines();
      sliceOutType desLines = imOut.getLines();
      lineInType lineIn;
      lineOutType lineOut;

      for(size_t s = 0; s < nSlices; ++s) {
#ifdef USE_OPEN_MP
#pragma omp parallel private( \
    lineIn, lineOut, l, v, previous_value, previous_label)
#endif // USE_OPEN_MP
        {
#ifdef USE_OPEN_MP
#pragma omp for
#endif // USE_OPEN_MP
          for(l = 0; l < nLines; ++l) {
            lineIn = srcLines[l + s * nSlices];
            lineOut = desLines[l + s * nSlices];
            previous_value = lineIn[0];
            previous_label = lineOut[0];
            for(v = 1; v < nPixels; ++v) {
              if(compareFunc(lineIn[v], previous_value)) {
                lineOut[v] = previous_label;
              } else {
                previous_value = lineIn[v];
                previous_label = lineOut[v];
              }
            }
          }
        }
      }
      return RES_OK;
    }

    compOperatorT compareFunc;

  protected:
    T2 labels;
    size_t labels_real;
    T2 max_value_label;
  };

  template <class T>
  struct lambdaEqualOperator {
    inline bool operator()(T &a, T &b) {
      bool retVal = a > b ? (a - b) <= lambda : (b - a) <= lambda;
      return retVal;
    }
    T lambda;
  };

#endif // SWIG

  template <class T1, class T2>
  size_t labelWithoutFunctor(const Image<T1> &imIn,
                             Image<T2> &imOut,
                             const StrElt &se = DEFAULT_SE) {
    // Checks
    ASSERT_ALLOCATED(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);

    // Typedefs
    typedef Image<T1> inT;
    typedef Image<T2> outT;
    typedef typename inT::lineType inLineT;
    typedef typename outT::lineType outLineT;

    // Initialisation.
    StrElt cpSe = se.noCenter();
    fill(imOut, T2(0));

    // Processing vars.
    size_t lblNbr = 0;
    size_t lblNbr_real = 0;
    size_t size[3];
    imIn.getSize(size);
    UINT sePtsNumber = cpSe.points.size();
    if(sePtsNumber == 0)
      return 0;
    queue<size_t> propagation;
    size_t o, nb_o;
    size_t x, x0, y, y0, z, z0;
    bool oddLine;
    // Image related.
    inLineT inP = imIn.getPixels();
    outLineT outP = imOut.getPixels();

    for(size_t s = 0; s < size[2]; ++s) {
      for(size_t l = 0; l < size[1]; ++l) {
        for(size_t p = 0; p < size[0]; ++p) {
          o = p + l * size[0] + s * size[0] * size[1];
          if(inP[o] != T1(0) && outP[o] == T2(0)) {
            ++lblNbr_real;
            ++lblNbr;
            if(lblNbr == (size_t)(ImDtTypes<T2>::max() - 1))
              lblNbr = 1;

            outP[o] = T2(lblNbr);
            propagation.push(o);
            do {
              o = propagation.front();
              propagation.pop();

              x0 = o % size[0];
              y0 = (o % (size[1] * size[0])) / size[0];
              z0 = o / (size[0] * size[1]);
              oddLine = cpSe.odd && y0 % 2;
              for(UINT pSE = 0; pSE < sePtsNumber; ++pSE) {
                x = x0 + cpSe.points[pSE].x;
                y = y0 + cpSe.points[pSE].y;
                z = z0 + cpSe.points[pSE].z;

                if(oddLine)
                  x += (y + 1) % 2;

                nb_o = x + y * size[0] + z * size[0] * size[1];
                if(x < size[0] && y < size[1] && z < size[2]
                   && outP[nb_o] != lblNbr && inP[nb_o] == inP[o]) {
                  outP[nb_o] = T2(lblNbr);
                  propagation.push(nb_o);
                }
              }
            } while(!propagation.empty());
          }
        }
      }
    }

    return lblNbr_real;
  }

  template <class T1, class T2>
  size_t labelWithoutFunctor2Partitions(const Image<T1> &imIn,
                                        const Image<T1> &imIn2,
                                        Image<T2> &imOut,
                                        const StrElt &se = DEFAULT_SE) {
    // Checks
    ASSERT_ALLOCATED(&imIn, &imIn2, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn2, &imOut);

    // Typedefs
    typedef Image<T1> inT;
    typedef Image<T2> outT;
    typedef typename inT::lineType inLineT;
    typedef typename outT::lineType outLineT;

    // Initialisation.
    StrElt cpSe = se.noCenter();
    fill(imOut, T2(0));

    // Processing vars.
    size_t lblNbr = 0;
    size_t lblNbr_real = 0;
    size_t size[3];
    imIn.getSize(size);
    UINT sePtsNumber = cpSe.points.size();
    if(sePtsNumber == 0)
      return 0;
    queue<size_t> propagation;
    size_t o, nb_o;
    size_t x, x0, y, y0, z, z0;
    bool oddLine;
    // Image related.
    inLineT inP = imIn.getPixels();
    inLineT in2P = imIn2.getPixels();
    outLineT outP = imOut.getPixels();

    for(size_t s = 0; s < size[2]; ++s) {
      for(size_t l = 0; l < size[1]; ++l) {
        for(size_t p = 0; p < size[0]; ++p) {
          o = p + l * size[0] + s * size[0] * size[1];
          if(inP[o] != T1(0) && outP[o] == T2(0)) {
            ++lblNbr_real;
            ++lblNbr;
            if(lblNbr == (size_t)ImDtTypes<T2>::max() - 1)
              lblNbr = 1;

            outP[o] = T2(lblNbr);
            propagation.push(o);
            do {
              o = propagation.front();
              propagation.pop();

              x0 = o % size[0];
              y0 = (o % (size[1] * size[0])) / size[0];
              z0 = o / (size[0] * size[1]);
              oddLine = cpSe.odd && y0 % 2;
              for(UINT pSE = 0; pSE < sePtsNumber; ++pSE) {
                x = x0 + cpSe.points[pSE].x;
                y = y0 + cpSe.points[pSE].y;
                z = z0 + cpSe.points[pSE].z;

                if(oddLine)
                  x += (y + 1) % 2;

                nb_o = x + y * size[0] + z * size[0] * size[1];
                if(x < size[0] && y < size[1] && z < size[2]
                   && outP[nb_o] != lblNbr && inP[nb_o] == inP[o]
                   && in2P[nb_o] == in2P[o]) {
                  outP[nb_o] = T2(lblNbr);
                  propagation.push(nb_o);
                }
              }
            } while(!propagation.empty());
          }
        }
      }
    }

    return lblNbr_real;
  }
  /** @endcond */
  /* @enddevdoc */

  /*
   *  ######  #    #  #    #   ####    #####     #     ####   #    #   ####
   *  #       #    #  ##   #  #    #     #       #    #    #  ##   #  #
   *  #####   #    #  # #  #  #          #       #    #    #  # #  #   ####
   *  #       #    #  #  # #  #          #       #    #    #  #  # #       #
   *  #       #    #  #   ##  #    #     #       #    #    #  #   ##  #    #
   *  #        ####   #    #   ####      #       #     ####   #    #   ####
   */
  /**
   * label() - Image labelization
   *
   * @param[in] imIn : input image
   * @param[out] imOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @note
   * The range of type @b T2 of the output image shall be big enough to
   * accomodate all label values (the number of disjoint regions in the input
   * image.
   */
  template <class T1, class T2>
  size_t label(const Image<T1> &imIn,
               Image<T2> &imOut,
               const StrElt &se = DEFAULT_SE) {
    if((void *)&imIn == (void *)&imOut) {
      // clone
      Image<T1> tmpIm(imIn, true);
      return label(tmpIm, imOut);
    }

    ASSERT_ALLOCATED(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);

    labelFunctGeneric<T1, T2> f;

    ASSERT((f._exec(imIn, imOut, se) == RES_OK), 0);

    size_t lblNbr = f.getLabelNbr();

    if(lblNbr > size_t(ImDtTypes<T2>::max()))
      std::cerr << "Label number exceeds data type max!" << std::endl;

    return lblNbr;
  }

  /**
   * lambdaLabel() - Lambda-flat zones labelization
   *
   * @details
   * In this mode of labeling, two neighbour pixels have the same label if
   * there is a path between them at which the value difference from pixel to
   * pixel doesn't exceed @b lambdaVal
   *
   * @param[in] imIn : input image
   * @param[in] lambdaVal : lambda value
   * @param[out] imOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @note
   * shall include an example of how to use this...
   */
  template <class T1, class T2>
  size_t lambdaLabel(const Image<T1> &imIn,
                     const T1 &lambdaVal,
                     Image<T2> &imOut,
                     const StrElt &se = DEFAULT_SE) {
    ASSERT_ALLOCATED(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);

    labelFunctGeneric<T1, T2, lambdaEqualOperator<T1>> f;
    f.compareFunc.lambda = lambdaVal;

    ASSERT((f._exec(imIn, imOut, se) == RES_OK), 0);

    size_t lblNbr = f.getLabelNbr();

    if(lblNbr > size_t(ImDtTypes<T2>::max()))
      std::cerr << "Label number exceeds data type max!" << std::endl;

    return lblNbr;
  }

  /**
   * fastLabel() - Image labelization (faster, use OpenMP)
   *
   * @param[in] imIn : input image
   * @param[out] imOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   */
  template <class T1, class T2>
  size_t fastLabel(const Image<T1> &imIn,
                   Image<T2> &imOut,
                   const StrElt &se = DEFAULT_SE) {
    ASSERT_ALLOCATED(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);

    labelFunctFast<T1, T2> f;

    ASSERT((f._exec(imIn, imOut, se) == RES_OK), 0);

    size_t lblNbr = f.getLabelNbr();

    if(lblNbr > size_t(ImDtTypes<T2>::max()))
      std::cerr << "Label number exceeds data type max!" << std::endl;

    return lblNbr;
  }

  /** @cond */
  /**
   * fastLambdaLabel() - Lambda-flat zones labelization (faster, use OpenMP)
   *
   * @param[in] imIn : input image
   * @param[in] lambdaVal : lambda expression
   * @param[out] imOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @note
   * shall include an example of how to use this...
   */
  template <class T1, class T2>
  size_t fastLambdaLabel(const Image<T1> &imIn,
                         const T1 &lambdaVal,
                         Image<T2> &imOut,
                         const StrElt &se = DEFAULT_SE) {
    ASSERT_ALLOCATED(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);

    labelFunctFast<T1, T2, lambdaEqualOperator<T1>> f;
    f.compareFunc.lambda = lambdaVal;

    ASSERT((f._exec(imIn, imOut, se) == RES_OK), 0);

    size_t lblNbr = f.getLabelNbr();

    if(lblNbr > size_t(ImDtTypes<T2>::max()))
      std::cerr << "Label number exceeds data type max!" << std::endl;

    return lblNbr;
  }
  /** @endcond */

  /** @cond */
  template <typename T>
  inline double maxMapValueDouble(map<T, double> &m) {
    return std::max_element(m.begin(), m.end(), map_comp_value_less())->second;
  }

  template <typename T>
  inline double minMapValueDouble(map<T, double> &m) {
    return std::min_element(m.begin(), m.end(), map_comp_value_less())->second;
  }
  /** @endcond */

  /**
   * labelWithProperty() - Image labelization with the value of the property
   * of each connected components in the @TT{imRegions image}. The intensity of
   * each pixel will correspond to the value of the property.
   *
   * @param[in] imRegions : an image with regions to be labeled
   * @param[in] imIn : image where to take values to evaluate properties
   * @param[out] imLabelOut : output image
   * @param[in] property : the property to use as label. One of :
   *      @TB{"area"}, @TB{"volume"}, @TB{"max"}, @TB{"min"},
   *      @TB{"mean"}, @TB{"stddev"}, @TB{"median"}, @TB{"mode"},
   *      @TB{"nbvalues"}, @TB{"entropy"}
   * @param[in] doRescale : values are rescaled to fullfit the range of the
   *    output image (@TB{T3}) : (@TT{[min, max] => [1, max(T3)]})
   * @param[in] scale : coefficient to multiply property result of each region
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @note
   * - The image @TT{imRegions} is labeled and blobs are assigned to it before
   *   evaluating the property of each region. The structuring element is needed
   *   just to the labeling operation.
   * - The same label can be assigned to different regions not connected regions
   *   in the image if they have the same property value.
   * - the number of regions in @TB{imRegions} shall be smaller than the maximum
   *   value of the type @TB{T3}
   * - The range of values of the output image (@TB{T3}) shall be big enough to
   *   accomodate all property values. If not, use parameters @{TT} or
   *   @TT{doRescale}
   * - @TT{scale} and @TT{doRescale} parameters are useful for visualisation
   *   purposes or when the goal isn't the real value of the property but their
   *   relative values (order of values). These parameters allows to use image
   *   with smaller data types. @TT{scale} is ignored when @TT{doRescale} is
   *   set.
   * - if the property is @TB{area} then @TT{imIn} and @TT{imRegions} are the
   *   same.
   */
  template <typename T1, typename T2, typename T3>
  size_t labelWithProperty(const Image<T1> &imRegions,
                           const Image<T2> &imIn,
                           Image<T3> &imLabelOut,
                           const string property = "area",
                           bool doRescale = false,
                           double scale = 1.,
                           const StrElt &se = DEFAULT_SE) {
    ASSERT_ALLOCATED(&imIn, &imRegions, &imLabelOut);
    ASSERT_SAME_SIZE(&imIn, &imRegions, &imLabelOut);

    vector<double> retVal(3);

    map<string, int> property2key
      = {{"area", 1},     {"volume", 2},  {"max", 3},    {"min", 4},
         {"mean", 5},     {"stddev", 6},  {"median", 7}, {"mode", 8},
         {"nbvalues", 9}, {"entropy", 10}};
    if(property2key.find(property) == property2key.end()) {
      ERR_MSG("Property unknown when calling function labelWithProperty");
      return 0;
    }
    int key = property2key[property];

    ImageFreezer freezer(imLabelOut);

    Image<T3> imLabel(imIn);

    size_t nl = label(imRegions, imLabel, se);
    if(nl > ImDtTypes<T3>::max()) {
      string msg
        = "Increase output image type to include value " + to_string(nl);
      ERR_MSG(msg);
      return 0;
    }
    if(nl == 0) {
      ERR_MSG("No labels returned");
      return nl;
    }
    map<T3, Blob> blobs = computeBlobs(imLabel, true);
    map<T3, double> markers;

    typedef typename std::map<T3, T2>::iterator itT2_T;
    typedef typename std::map<T3, std::vector<T2>>::iterator itT2Vec_T;
    typedef typename std::map<T3, double>::iterator itD_T;
    typedef typename std::map<T3, std::vector<double>>::iterator itDVec_T;

    switch(key) {
      case 1: {
        // area
        map<T3, double> values = blobsArea(blobs);
        for(itD_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second;
        }
      } break;
      case 2: {
        // volume
        map<T3, double> values = blobsVolume(imIn, blobs);
        for(itD_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second;
        }
      } break;
      case 3: {
        // min
        map<T3, T2> values = blobsMinVal(imIn, blobs);
        for(itT2_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second;
        }
      } break;
      case 4: {
        // max
        map<T3, T2> values = blobsMaxVal(imIn, blobs);
        for(itT2_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second;
        }
      } break;
      case 5: {
        // mean
        map<T3, std::vector<double>> values = blobsMeanVal(imIn, blobs);
        for(itDVec_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = (iter->second)[0];
        }
      } break;
      case 6: {
        // stddev
        map<T3, std::vector<double>> values = blobsMeanVal(imIn, blobs);
        for(itDVec_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = (iter->second)[1];
        }
      } break;
      case 7: {
        // median
        map<T3, T2> values = blobsMedianVal(imIn, blobs);
        for(itT2_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second;
        }
      } break;
      case 8: {
        // median
        map<T3, T2> values = blobsModeVal(imIn, blobs);
        for(itT2_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second;
        }
      } break;
      case 9: {
        // values count
        map<T3, vector<T2>> values = blobsValueList(imIn, blobs);
        for(itT2Vec_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second.size();
        }
      } break;
      case 10: {
        // entropy
        map<T3, double> values = blobsEntropy(imIn, blobs);
        for(itD_T iter = values.begin(); iter != values.end(); ++iter) {
          markers[iter->first] = iter->second;
        }
      } break;
      default:
        return 0;
        break;
    }

    ASSERT(!markers.empty());

    double maxV = maxMapValueDouble(markers);
    double minV = minMapValueDouble(markers);
    double maxT = double(ImDtTypes<T3>::max());

    retVal[1] = minV;
    retVal[2] = maxV;

    if(doRescale) {
      if(maxV > minV) {
        double k = (maxT - 1.) / (maxV - minV);
        for(auto it = markers.begin(); it != markers.end(); it++) {
          if(it->second > 0)
            it->second = 1. + k * (it->second - minV);
        }
        cout << "  Values where rescaled :" << endl;
        cout << "    Min : \t" << minV << "\t=> " << 1. << endl;
        cout << "    Max : \t" << maxV << "\t=> " << maxT << endl;
        maxV = maxMapValueDouble(markers);
      }
    } else {
      if(abs(scale - 1.) > 0.001) {
        for(auto it = markers.begin(); it != markers.end(); it++)
          it->second *= scale;
      }
    }
    if(maxV > maxT) {
      stringstream ss;
      ss << "Max " << property << " value (" << maxV
         << ") exceeds data type upper limit (" << maxT << ")";
      ERR_MSG(ss.str());
      return 0;
    }

    if(applyLookup(imLabel, markers, imLabelOut) != RES_OK)
      return 0;

    retVal[0] = nl;
    return nl;
  }

  /**
   * labelWithArea() - Image labelization with the size (area) of each connected
   * components
   *
   * @param[in] imIn : input image
   * @param[out] imOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @warning This function is obsolete. Use labelWithProperty() :
   @BeginCpp
      labelWithProperty(imIn, imIn, imOut, "area", false, 1., se);
   @EndCpp
   * @note
   * - The range of values of the output image (@TB{T2}) shall be big enough to
   * accomodate all label values.
   * - The same value can be assigned to different disconnected regions
   * in the image if they have the same area.
   */
  template <class T1, class T2>
  size_t labelWithArea(const Image<T1> &imIn,
                       Image<T2> &imOut,
                       const StrElt &se = DEFAULT_SE) {
#if 1
    return labelWithProperty(imIn, imIn, imOut, "area", false, 1., se);
#else
    ASSERT_ALLOCATED(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);

    ImageFreezer freezer(imOut);

    Image<T2> imLabel(imIn);

    ASSERT(label(imIn, imLabel, se) != 0);
    map<T2, double> areas = blobsArea(imLabel);
    ASSERT(!areas.empty());

    double maxV
      = std::max_element(areas.begin(), areas.end(), map_comp_value_less())
          ->second;
    ASSERT((maxV < double(ImDtTypes<T2>::max())),
           "Areas max value exceeds data type max!", 0);

    ASSERT(applyLookup(imLabel, areas, imOut) == RES_OK);

    return RES_OK;
#endif
  }

  /**
   * labelWithVolume() - Image labelization with the volume (sum of values) of
   * each connected components in the imLabelIn image
   *
   * @param[in] imIn : input image
   * @param[in] imLabelIn : an image with regions defined in @b imIn
   * @param[out] imLabelOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @warning This function is obsolete. Use labelWithProperty()
   @BeginCpp
      labelWithProperty(imLabelIn, imIn, imLabelOut, "volume", false, 1., se);
   @EndCpp
   * @note
   * - The range of values of the output image (@TB{T2}) shall be big enough to
   * accomodate all label values.
   * - The same value can be assigned to different disconnected regions
   * in the image if they have the same volume.
   */
  template <class T1, class T2>
  size_t labelWithVolume(const Image<T1> &imIn,
                         const Image<T2> &imLabelIn,
                         Image<T2> &imLabelOut,
                         const StrElt &se = DEFAULT_SE) {
#if 1
    return labelWithProperty(
      imLabelIn, imIn, imLabelOut, "volume", false, 1., se);
#else
    ASSERT_ALLOCATED(&imIn, &imLabelOut);
    ASSERT_SAME_SIZE(&imIn, &imLabelOut);

    ImageFreezer freezer(imLabelOut);

    Image<T2> imLabel(imIn);

    ASSERT(label(imLabelIn, imLabel, se) != 0);
    label(imLabelIn, imLabel, se);
    bool onlyNonZeros = true;
    map<T2, Blob> blobs = computeBlobs(imLabel, onlyNonZeros);
    map<T2, double> volumes = blobsVolume(imIn, blobs);
    ASSERT(!volumes.empty());

    double maxV
      = std::max_element(volumes.begin(), volumes.end(), map_comp_value_less())
          ->second;
    cout << maxV << endl;
    ASSERT((maxV < double(ImDtTypes<T2>::max())),
           "Volumes max value exceeds data type max!", 0);

    ASSERT(applyLookup(imLabel, volumes, imLabelOut) == RES_OK);

    return RES_OK;
#endif
  }

  /**
   * labelwithMax() - Image labelization with the maximum values of each
   * connected components in the imLabelIn image
   *
   * @param[in] imIn : input image
   * @param[in] imLabelIn : an image with regions defined in @b imIn
   * @param[out] imLabelOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @warning This function is obsolete. Use labelWithProperty()
   @BeginCpp
      labelWithProperty(imLabelIn, imIn, imLabelOut, "max", false, 1., se);
   @EndCpp
   * @note
   * - The range of values of the output image (@TB{T2}) shall be big enough to
   * accomodate all label values.
   * - The same value can be assigned to different disconnected regions
   * in the image if they have the maximum value.
   */
  template <class T1, class T2>
  size_t labelWithMax(const Image<T1> &imIn,
                      const Image<T2> &imLabelIn,
                      Image<T2> &imLabelOut,
                      const StrElt &se = DEFAULT_SE) {
#if 1
    return labelWithProperty(imLabelIn, imIn, imLabelOut, "max", false, 1., se);
#else
    ASSERT_ALLOCATED(&imIn, &imLabelOut);
    ASSERT_SAME_SIZE(&imIn, &imLabelOut);

    ImageFreezer freezer(imLabelOut);

    Image<T2> imLabel(imIn);

    ASSERT(label(imLabelIn, imLabel, se) != 0);
    label(imLabelIn, imLabel, se);
    bool onlyNonZeros = true;
    map<T2, Blob> blobs = computeBlobs(imLabel, onlyNonZeros);
    map<T2, T1> markers = blobsMaxVal(imIn, blobs);
    ASSERT(!markers.empty());

    double maxV
      = std::max_element(markers.begin(), markers.end(), map_comp_value_less())
          ->second;
    // cout << maxV << endl;
    ASSERT((maxV < double(ImDtTypes<T2>::max())),
           "Markers max value exceeds data type max!", 0);

    ASSERT(applyLookup(imLabel, markers, imLabelOut) == RES_OK);

    return RES_OK;
#endif
  }

  /**
   * labelWithMean() - Image labelization with the mean values of each connected
   * components in the imLabelIn image
   *
   * @param[in] imIn : input image
   * @param[in] imLabelIn : an image with regions defined in @b imIn
   * @param[out] imLabelOut : output image
   * @param[in] se : structuring element
   * @returns the number of labels (or 0 if error)
   *
   * @warning This function is obsolete. Use labelWithProperty()
   @BeginCpp
      labelWithProperty(imLabelIn, imIn, imLabelOut, "mean", false, 1., se);
   @EndCpp
   * @note
   * - The range of values of the output image (@TB{T2}) shall be big enough to
   * accomodate all label values.
   * - The same label can be assigned to different regions not connected regions
   * in the image if they have the same mean value.
   */
  template <class T1, class T2>
  size_t labelWithMean(const Image<T1> &imIn,
                       const Image<T2> &imLabelIn,
                       Image<T1> &imLabelOut,
                       const StrElt &se = DEFAULT_SE) {
#if 1
    return labelWithProperty(
      imLabelIn, imIn, imLabelOut, "mean", false, 1., se);
#else
    ASSERT_ALLOCATED(&imIn, &imLabelOut);
    ASSERT_SAME_SIZE(&imIn, &imLabelOut);

    ImageFreezer freezer(imLabelOut);

    Image<T2> imLabel(imIn);

    ASSERT(label(imLabelIn, imLabel, se) != 0);
    label(imLabelIn, imLabel, se);
    bool onlyNonZeros = true;
    map<T2, Blob> blobs = computeBlobs(imLabel, onlyNonZeros);
    map<T2, std::vector<double>> meanValsStd = blobsMeanVal(imIn, blobs);
    map<T2, double> markers;

    for(typename std::map<T2, std::vector<double>>::iterator iter
        = meanValsStd.begin();
        iter != meanValsStd.end(); ++iter) {
      markers[iter->first] = (iter->second)[0];
      //    cout << "iter->first = " << iter->first << "  iter->second[0] " <<
      // iter->second[0] << endl;
    }

    ASSERT(!markers.empty());

    double maxV
      = std::max_element(markers.begin(), markers.end(), map_comp_value_less())
          ->second;
    ASSERT((maxV < double(ImDtTypes<T2>::max())),
           "Markers max value exceeds data type max!", 0);

    ASSERT(applyLookup(imLabel, markers, imLabelOut) == RES_OK);

    return RES_OK;
#endif
  }

  /** @cond */
  template <class T1, class T2>
  class neighborsFunct : public MorphImageFunctionBase<T1, T2> {
  public:
    typedef MorphImageFunctionBase<T1, T2> parentClass;

    virtual inline void processPixel(size_t pointOffset,
                                     vector<int> &dOffsetList) {
      vector<T1> vals;
      UINT nbrValues = 0;
      vector<int>::iterator dOffset = dOffsetList.begin();
      while(dOffset != dOffsetList.end()) {
        T1 val = parentClass::pixelsIn[pointOffset + *dOffset];
        if(find(vals.begin(), vals.end(), val) == vals.end()) {
          vals.push_back(val);
          nbrValues++;
        }
        dOffset++;
      }
      parentClass::pixelsOut[pointOffset] = T2(nbrValues);
    }
  };
  /** @endcond */

  /**
   * neighbors() - Neighbors count
   *
   * Return for each pixel the number of different values in the neighborhood.
   *
   * Usefull in order to find interfaces or multiple points between basins (see
   * basins())
   *
   * @param[in] imIn : input image
   * @param[out] imOut : output image
   * @param[in] se : structuring element
   */
  template <class T1, class T2>
  RES_T neighbors(const Image<T1> &imIn,
                  Image<T2> &imOut,
                  const StrElt &se = DEFAULT_SE) {
    ASSERT_ALLOCATED(&imIn, &imOut);
    ASSERT_SAME_SIZE(&imIn, &imOut);

    neighborsFunct<T1, T2> f;

    ASSERT((f._exec(imIn, imOut, se) == RES_OK));

    return RES_OK;
  }

  /** @} */

} // namespace smil

#endif // _D_MORPHO_LABEL_HPP
