#ifndef __MORPHEE_FILTERS_HPP__
#define __MORPHEE_FILTERS_HPP__

#include <morphee/image/include/imageInterface.hpp>
#include <morphee/common/include/commonTypes.hpp>

namespace morphee
{
  //! namespace for filtering operations
  namespace filters
  {
    /*!
     * @defgroup filters_group Simple (non-morphologic) filters
     * @brief Functions that apply simple filters, such as convolutions, on
     * images
     *
     * @{
     */

#ifdef __MORPHEE_DYNAMIC_LIBRARY
#ifdef __MORPHEE_DYNAMIC_MORPHEEFILTERS_LIBRARY_EXPORT
#define __Mfil __declspec(dllexport)
#pragma message(" - Exporting Morphee Filters")
#else
#define __Mfil __declspec(dllimport)
#pragma message(" - Dynamic link to Morphee Filters")
#endif
#else
#define __Mfil
#endif
    /*!
     * @defgroup convolution_group Convolution filters
     *
     * @ingroup filters_group
     *
     * @brief Filters using convolution kernels
     *
     * This module holds all the filters that use a convolution kernel that
     * is applied to an image.
     *
     * @{
     */

    //! Filtering of an image by a gaussian kernel
    __Mfil RES_C ImGaussianFilter(const ImageInterface *, INT32 filterRadius,
                                  ImageInterface *);

    //! Filtering of an image by a Laplace kernel (2nd derivative)
    __Mfil RES_C ImLaplacianFilter(const ImageInterface *, ImageInterface *);

    //! Filtering of an image by a simple derivative kernel in direction X
    //! [-1,0,1] (1nd derivative)
    __Mfil RES_C ImDifferentialGradientX(const ImageInterface *imin,
                                         ImageInterface *imOut);

    //! Filtering of an image by a simple derivative kernel in direction Y
    //! [-1,0,1] (1nd derivative)
    __Mfil RES_C ImDifferentialGradientY(const ImageInterface *imin,
                                         ImageInterface *imOut);

    //! @} defgroup convolution_group

    /*! @defgroup diffusion_group Diffusion filters
     *
     * @ingroup filters_group
     *
     * @brief Diffusion tools and functions
     * The diffusion group provides some tools in order to quickly implement new
     * diffusion filters. It provides also some interesting existing diffusion
     * filters such as the one defined by the heat propagation, Perona Malik or
     * Weikert.
     *
     * @{
     */
    //! Compute a (numeric) gradient
    __Mfil RES_C Gradient(const ImageInterface *imin,
                          std::vector<ImageInterface *> &vect);

    //! Perform a number of iterations of the heat diffusion equation on the
    //! image
    __Mfil RES_C HeatDiffusion(const ImageInterface *imin,
                               const morphee::UINT32 nosteps,
                               const F_SIMPLE step_value,
                               ImageInterface *imOut);

    //! Perform a number of iterations of the Perona-Malik diffusion equation on
    //! the image
    __Mfil RES_C PeronaMalikDiffusion(const ImageInterface *imin,
                                      const morphee::UINT32 nosteps,
                                      const F_SIMPLE step_value,
                                      const F_SIMPLE lambda,
                                      ImageInterface *imOut);

    //! Perform a number of iterations of the Weickert diffusion equation on the
    //! image
    __Mfil RES_C WeickertDiffusion(const ImageInterface *imin,
                                   const morphee::UINT32 nosteps,
                                   const F_SIMPLE step_value,
                                   const F_SIMPLE lambda, const F_SIMPLE m,
                                   const F_SIMPLE cm, ImageInterface *imOut);

    //! @} diffusion_group

    /*!
     * @defgroup noise_group Noisifying filters
     *
     * These function are available to <em>add</em> noise to images. Strange as
     * this may seem, sometimes it is useful. :)
     *
     * @{
     */

    //! Set the seed for random algorithms (for repeatable behaviors). s must be
    //! strictly positive
    __Mfil void setSeed(unsigned long int s);

    //! Returns the seed for random number generators
    __Mfil unsigned long int getSeed();

    //! Add "salt and pepper" noise to an image. Noise will be
    //! black or white pixels (50% each). The number of noisy
    //! pixels depends on the given the frequency.
    __Mfil RES_C ImAddNoiseSaltAndPepper(const ImageInterface *imIn,
                                         const F_DOUBLE frequency,
                                         ImageInterface *imOut);

    //! Adds gaussian noise with the given std deviation to an image
    __Mfil RES_C ImAddNoiseGaussian(const ImageInterface *imIn,
                                    const F_DOUBLE sigma,
                                    ImageInterface *imOut);

    /*!
     * @brief Adds poissonian noise to an image
     *
     * Each pixel of the output image will be the realisation of a
     * Poisson random variable whose mean is set to the value of
     * the corresponding pixel in the input image.
     *
     * @warning Defining a Poisson variable for negative values or
     * floating point values is absurd, really ! Here, when
     * negative pixels are encoutered whe chosed to work on the
     * absolute value of the pixel (hence inversing the axis for
     * the negative pixels) but this is not guaranteed to have any
     * sense ! If you want to do something meaningfull be sure
     * you're using this function on @b image @b made of @b integers with
     * @b positive value @b only !
     *
     */
    __Mfil RES_C ImAddNoisePoissonian(const ImageInterface *imIn,
                                      ImageInterface *imOut);

    //! @} noise_group

    /*
     * @brief Performs a brut decimation over the provided image. The
     * "x/y/z_factor" are division factors (and should be >= 1)
     */
    __Mfil ImageInterface *
    ImSimpleDecimator(const ImageInterface *imIn,
                      const std::vector<F_SIMPLE> &factors);

    /*
     * @brief Performs a decimation of a image on HLS color space, each output
     * pixel takes the value of the median of an input block, computed through a
     * distance function on HLS. The "x/y/z_factor" are division factors (and
     * should be >= 1)
     */
    __Mfil ImageInterface *ImHLSDecimator(const ImageInterface *imIn,
                                          const std::vector<F_SIMPLE> &factors);

    /*
     * @brief Performs a zoom of the input image. The zoom factors are not
     * necessarly integers
     */
    __Mfil ImageInterface *ImZoom(const ImageInterface *imIn,
                                  const std::vector<F_SIMPLE> &factors);

    __Mfil RES_C ImHarrisOperator(const ImageInterface *imIn, UINT32 gaussSize,
                                  ImageInterface *imOut);

    /*
     * @brief Performs a noise reduction following the Lee paper
     */
    __Mfil RES_C ImSigmaFilter(const ImageInterface *imIn, const UINT8 radius,
                               const double sigma,
                               const double percentageNbMinPixel,
                               const bool excludeOutlier,
                               ImageInterface *imOut);

    /*! @brief FastBilateral filter : Smooth the picture while conserving the
     * edges
     * @param[in]  imIn the initial image
     * @param[in]	Method : 1 for a gaussian window. Otherwize it is a tukey
     * window.
     * @param[in]  nS: size of the neigbourhood (Outside this window, the
     * gaussian is null) (common value 5)
     * @param[in]  EctS: standard deviation (std) for the spatial filtering
     * (common value : 3 or 5)
     * @param[in]  EctG: standard deviation (std) for the gray level filtering
     * (common value :20 or 40)
     * @param[out] imOut Result of the bilateral filter of size Lenght
     */
    __Mfil RES_C ImFastBilateralFilter(const ImageInterface *imIn,
                                       const UINT8 Method, const UINT8 nS,
                                       const UINT32 EctS, const UINT32 EctG,
                                       ImageInterface *imOut);

    /* @brief Performs the Kuwahara Filter, a noise-reduction filter that
     * preserves edges. a  a  ab   b  b a  a  ab   b  b ac ac abcd bd bd c  c cd
     * d  d c  c  cd   d  d In the case of a 5x5 sampling window, the mean
     * brightness and the variance of each of the four 3x3 regions (a, b, c, d),
     * are calculated and the value of the center pixel (abcd) is set to the
     * mean value of the region that with the smallest variance.
     */
    __Mfil RES_C ImKuwaharaFilter(const ImageInterface *imIn, const int radius,
                                  ImageInterface *imOut);

    /*
     * @brief Performs a noise an efficient reduction using clustering
     */
    __Mfil RES_C ImMeanShift(const ImageInterface *imIn, const UINT8 radius,
                             const int tonalDistance, ImageInterface *imOut);

    /*! @brief Canny's filter : Grayscale optimal edge detection.
     * @param[in]  imIn the initial image
     * @param[in]  Sigma standard deviation of the noise
     * @param[out] imOut Edges of imIn
     */
    __Mfil RES_C ImCannyEdgeDetection(const ImageInterface *imIn,
                                      const double Sigma,
                                      ImageInterface *imOut);

    /*! @brief Deriche's filter : Grayscale optimal edge detection.
     * @param[in]  imIn the initial image
     * @param[in]  Alpha [0 1] Parameter of the filter
     * @param[out] imOut Edges of imIn
     */
    __Mfil RES_C ImDericheEdgeDetection(const ImageInterface *imIn,
                                        const double Alpha,
                                        ImageInterface *imOut);

    /*! @brief Gabor's filter :
"ImGaborFilterConvolution(imIn,sigma,theta,lambda,psi,gamma,imOut)n [comment]:
Texture analysis with the computation of a Gabor filter. [in] imIn: Input Image
[in] sigma : size of the gaussian
[in] theta : orientation of the kernel (in rad)
[in] lambda : frequency of the cos
[in] psi : phase of the cos (in rad)
[in] gamma : anisotropy of the kernel
[out] imOut: output Image (must be F_SIMPLE or F_DOUBLE).
[example]:
\t--> Typical use of this filter :
ImGaborFilterConvolution(imIn,5,0,20,0,1,imOut)\n");
    * @param[in]  imIn the initial image
    * @param[in]  Sigma standard deviation of the noise
    * @param[out] imOut Edges of imIn
    */
    __Mfil RES_C ImGaborFilterConvolution(const ImageInterface *imIn,
                                          double sigma, double theta,
                                          double lambda, double psi,
                                          double gamma, ImageInterface *imOut);

    /* @brief Gabor's filter
      "ImGaborFilterConvolution_Normalized(imIn,sigma,theta,lambda,psi,gamma,Min,Max,imOut)n\
          [comment]: Texture analysis with the computation of a Gabor filter.
  [in] imIn: Input Image
  [in] sigma : size of the gaussian
  [in] theta : orientation of the kernel (in rad)
  [in] lambda : frequency of the cos
  [in] psi : phase of the cos (in rad)
  [in] gamma : anisotropy of the kernel
  [in] Min : for the normalization (return from the function _Auto_Normalized)
  [in] Max : for the normalization (return from the function _Auto_Normalized)
  [out] imOut: output Image (normalised between Min and Max).
  [example]:
  \t--> Typical use of this filter :
  ImGaborFilterConvolution_Normalized(imIn,5,0,20,0,1,minimum,maximum,imOut)\n");
  */
    __Mfil RES_C ImGaborFilterConvolution_Normalized(const ImageInterface *imIn,
                                                     double sigma, double theta,
                                                     double lambda, double psi,
                                                     double gamma, double Min,
                                                     double Max,
                                                     ImageInterface *imOut);

    /* @brief Gabor's filter
"ImGaborFilterConvolution_Auto_Normalized(imIn,sigma,theta,lambda,psi,gamma,imOut)n\
        [comment]: Texture analysis with the computation of a Gabor filter. The
normalization is automatic [in] imIn: Input Image [in] sigma : size of the
gaussian [in] theta : orientation of the kernel (in rad) [in] lambda : frequency
of the cos [in] psi : phase of the cos (in rad) [in] gamma : anisotropy of the
kernel [in] Min : for the normalization (return from the function
_AutoNormalized) [in] Max : for the normalization (return from the function
_AutoNormalized) [out] imOut: output normalized image . [out] Min Max in a
python list to apply the same normalization . [example]: \t--> Typical use of
this filter :
ImGaborFilterConvolution_Auto_Normalized(imIn,5,0,20,0,1,imOut)\n");
*/
    __Mfil RES_C ImGaborFilterConvolution_Auto_Normalized(
        const ImageInterface *imIn, double sigma, double theta, double lambda,
        double psi, double gamma, double *Min, double *Max,
        ImageInterface *imOut);

    __Mfil RES_C ImNormalized(const ImageInterface *imIn, double Value,
                              ImageInterface *imOut);

    /*"createGaborKernel(sigma,theta,lambda,psi,gamma)-> New imagen\
        [comment]: return a gabor kernel for the fft
[in] sigma : size of the gaussian
[in] theta : orientation of the kernel (in rad)
[in] lambda : frequency of the cos
[in] psi : phase of the cos (in rad)
[in] gamma : anisotropy of the kernel
[out] imOut: gabor kernel (double)
[example]:
\t--> Typical use of this function : imKernel =
createGaborKernel(5,0,20,0,1)\n",
        boost::python::return_value_policy<boost::python::manage_new_object>());
    */
    __Mfil RES_C createGaborKernel(ImageInterface *imOut, double sigma,
                                   double theta, double lambda, double psi,
                                   double gamma);

    /*"ImDisplayKernel(imIn,imOut)n\
        [comment]: Normalization of the kernel between 0 and 255.
[in] imIn: Input Image
[out] imOut: output normalized image UINT8.
[example]:
\t--> Typical use of this function : ImDisplayKernel(imIn,imOut)\n");
*/
    __Mfil RES_C ImDisplayKernel(const ImageInterface *imIn,
                                 ImageInterface *imOut);

    //! @} filters_group

  } // namespace filters
} // namespace morphee
#endif
