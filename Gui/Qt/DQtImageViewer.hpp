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

#ifndef _D_QT_IMAGE_VIEWER_HPP
#define _D_QT_IMAGE_VIEWER_HPP

#include <QApplication>
#include <QGraphicsSceneEvent>

#include "Gui/include/private/DImageViewer.hpp"
#include "Core/include/DTypes.h"

#include "PureQt/ImageViewerWidget.h"

class PlotWidget;

namespace smil
{
#define BASE_QT_VIEWER ImageViewerWidget

  template <class T>
  class Image;

  /**
   * @ingroup Gui
   */
  /*@{*/

  /**
   * Qt image viewer
   *
   * Requires the Qt libraries. To use it, you must set the option USE_QT to ON.
   *
   * Keyboard shortcuts:
   * @li @b Z/A Zoom In/Out
   * @li @b V Show/Hide the value of the pixel under the mouse cursor
   * @li @b M Show/Hide the magnifying window
   * @li @b R Enable/disable the auto-adjust range feature (allows to
   * automatically adjust range for low contrasted images).
   * @li @b H SHow the histogram window (requires the Qwt library)
   * @li @b P SHow the plot window (requires the Qwt library)
   */
  template <class T>
  class QtImageViewer
#ifndef SWIG
      : public ImageViewer<T>,
        public BASE_QT_VIEWER
#endif // SWIG
  {
  public:
    using parentClass = ImageViewer<T>;
    QtImageViewer();
    QtImageViewer(Image<T> &im);
    ~QtImageViewer() override;

    void setImage(Image<T> &im) override;
    void hide() override;
    void show() override;
    void showLabel() override;
    bool isVisible() override;
    void setName(const char *_name) override;
    void update() override;
    void saveSnapshot(const char *fileName) override;
    void redrawImage() override // ImageViewerWidget
    {
      update();
    }
    void updateIcon()
    {
      if (!this->image)
        return;

      BASE_QT_VIEWER::updateIcon();
    }
    void drawOverlay(const Image<T> &im) override;
    void clearOverlay() override
    {
      BASE_QT_VIEWER::clearOverlay();
    }
    RES_T getOverlay(Image<T> &img) override;

    //! Set the color table as a 8bits RGB map (keys between 0 and 255)
    void setLookup(const std::map<UINT8, RGB> &lut) override;
    void resetLookup() override;

    void setCurSlice(int) override
    {
      this->update();
    }

    void setLabelImage(bool val) override;

#ifdef USE_QWT
    void displayHistogram(bool update = false) override;
    void displayProfile(bool update = false) override;
#endif // USE_QWT

  protected:
    void displayPixelValue(size_t x, size_t y, size_t z) override;
    void displayMagnifyView(size_t x, size_t y, size_t z) override;
    void drawImage() override;
    void overlayDataChanged(bool triggerEvents = true) override;
    //     ImageViewerWidget *qtViewer;
    //     ImageViewer *qtViewer;
    void dropEvent(QDropEvent *de) override;

#ifdef USE_QWT
    PlotWidget *histoPlot;
    PlotWidget *profilePlot;
#endif // USE_QWT
  };

  /*@{*/

} // namespace smil

#endif // _D_QT_IMAGE_VIEWER_HPP
