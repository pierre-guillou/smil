/* __HEAD__
 * Copyright (c) 2011-2016, Matthieu FAESSEL and ARMINES
 * Copyright (c) 2017-2024, Centre de Morphologie Mathematique
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
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Description :
 *   2D Gabor filter implementation by Vincent Morard
 *
 * History :
 *   - XX/XX/XXXX - by Vincent Morard
 *     Just created it
 *   - 21/02/2019 - by Jose-Marcio Martins da Cruz
 *     Formatting and removing some warnings and minor differences
 *
 * __HEAD__ - Stop here !
 */

#ifndef _DGABOR_FILTER_H_
#define _DGABOR_FILTER_H_

#include "Core/include/DCore.h"

namespace smil {
  /**
   * @ingroup   AddonFilters
   * @defgroup  AddonGaborFilter        Gabor Filter (2D)
   *
   * @brief A 2D Gabor filter implementation
   *
   * Texture analysis with the computation of a Gabor filter
   *
   * @see Gabor Filter <a href="https://en.wikipedia.org/wiki/Gabor_filter">
   *       on Wikipedia</a>
   *
   * @author Vincent Morard
   * @author Jose-Marcio Martins da Cruz (port from @c Morph-M)
   * @{ */

  /**
   * @brief gaborFilterConvolution Gabor Filter
   * @param[in] imIn : input Image
   * @param[in] sigma : size of the gaussian
   * @param[in] theta : orientation of the kernel (in rad)
   * @param[in] lambda : frequency of the cos
   * @param[in] psi : phase of the cos (in rad)
   * @param[in] gamma : anisotropy of the kernel
   * @param[out] imOut : output Image (must be F_SIMPLE or F_DOUBLE ???)
   */
  template <class T>
  RES_T gaborFilterConvolution(const Image<T> &imIn,
                               double sigma,
                               double theta,
                               double lambda,
                               double psi,
                               double gamma,
                               Image<T> &imOut);

  /**
   * @brief gaborFilterConvolutionNorm Gabor Filter (normalized between
   *   Min and Max)
   * @param[in] imIn : input Image
   * @param[in] sigma : size of the gaussian
   * @param[in] theta : orientation of the kernel (in rad)
   * @param[in] lambda : frequency of the cos
   * @param[in] psi : phase of the cos (in rad)
   * @param[in] gamma : anisotropy of the kernel
   * @param[in] Min :
   * @param[in] Max :
   * @param[out] imOut : output Image (must be F_SIMPLE or F_DOUBLE ???)
   * @param[out] imGabor : output Image (must be F_SIMPLE or F_DOUBLE ???)
   */
  template <class T>
  RES_T gaborFilterConvolutionNorm(const Image<T> &imIn,
                                   double sigma,
                                   double theta,
                                   double lambda,
                                   double psi,
                                   double gamma,
                                   double Min,
                                   double Max,
                                   Image<T> &imOut,
                                   Image<T> &imGabor);

  /**
   * @brief gaborFilterConvolutionNormAuto Gabor Filter (automatically
   * normalized)
   * @param[in] imIn : input Image
   * @param[in] sigma : size of the gaussian
   * @param[in] theta : orientation of the kernel (in rad)
   * @param[in] lambda : frequency of the cos
   * @param[in] psi : phase of the cos (in rad)
   * @param[in] gamma : anisotropy of the kernel
   * @param[out] Min :
   * @param[out] Max :
   * @param[out] imOut : output Image (must be F_SIMPLE or F_DOUBLE ???)
   * @param[out] imGabor : output Image (must be F_SIMPLE or F_DOUBLE ???)
   */
  template <class T>
  RES_T gaborFilterConvolutionNormAuto(const Image<T> &imIn,
                                       double sigma,
                                       double theta,
                                       double lambda,
                                       double psi,
                                       double gamma,
                                       double *Min,
                                       double *Max,
                                       Image<T> &imOut,
                                       Image<T> &imGabor);
  /** @} */
} // namespace smil

#include "private/filterGabor/filterGabor.hpp"

#endif // _DGABOR_FILTER_H_
