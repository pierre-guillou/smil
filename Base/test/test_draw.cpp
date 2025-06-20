/*
 * Smil
 * Copyright (c) 2011-2015 Matthieu Faessel
 *
 * This file is part of Smil.
 *
 * Smil is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Smil is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Smil.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include "Core/include/DCore.h"
#include "DImageDraw.hpp"

#ifdef USE_FREETYPE
#include <fstream>
#endif // USE_FREETYPE

using namespace smil;

class Test_DrawCircle : public TestCase
{
  virtual void run()
  {
    Image<UINT8> im1(20, 15);
    Image<UINT8> im2(20, 15);
    fill(im1, UINT8(127));

    TEST_ASSERT(drawCircle(im1, 10, 7, 5) == RES_OK);

    UINT8 vec[20 * 15] = {
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 255, 255, 255, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 255, 127,
        127, 127, 255, 255, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 255, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127, 127, 127,
        127, 127, 255, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 255, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 255, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 255, 127, 127, 127, 127, 127, 127, 127, 255, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127,
        127, 127, 127, 127, 255, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 255, 255, 127, 127, 127, 255, 255, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 255, 255,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127,
    };

    im2 << vec;

    TEST_ASSERT(im1 == im2);

    if (retVal != RES_OK)
      im1.printSelf(1);
  }
};

class Test_DrawSphere : public TestCase
{
  virtual void run()
  {
    Image<UINT8> im1(20, 15);
    Image<UINT8> im2(20, 15);
    fill(im1, UINT8(127));

    TEST_ASSERT(drawSphere(im1, 10, 7, 0, 7) == RES_OK);

    UINT8 vec[20 * 15] = {
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255,
        255, 255, 255, 255, 255, 255, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 127, 127, 127, 127, 127, 127, 127, 127,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 127,
        127, 127, 127, 127, 127, 127, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 127, 127, 127, 127, 127, 127, 127, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 127, 127, 127,
        127, 127, 127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 127, 127, 127, 127, 127, 127, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 127, 127, 127, 127, 127,
        127, 127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 127, 127, 127, 127, 127, 127, 127, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 127, 127, 127, 127, 127, 127, 127,
        127, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 255, 255, 255, 255, 255, 255, 255, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127,
        127, 127, 127, 127, 127, 127,
    };

    im2 << vec;

    TEST_ASSERT(im1 == im2);

    if (retVal != RES_OK)
      im1.printSelf(1);
  }
};

class Test_DrawText : public TestCase
{
  virtual void run()
  {
#ifdef USE_FREETYPE

    Image<UINT8> im1(20, 15);
    Image<UINT8> im2(20, 15);
    fill(im1, UINT8(127));

#define N_FONT_PATHS 3
    string font_search_paths[N_FONT_PATHS] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", // Ubuntu
        "/usr/share/fonts/TTF/DejaVuSans.ttf",             // ArchLinux
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",          // CentOS
    };
    string fpath;
    for (int i = 0; i < N_FONT_PATHS; i++) {
      fpath = font_search_paths[i];
      // trying to open the font path to check if it exists
      ifstream res(fpath.c_str());
      if (res.good()) {
        res.close();
        break;
      }
    }

    // JOE TEST_ASSERT(drawText(im1, 4, 10, "ok",
    // "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf", 10)==RES_OK);
    TEST_ASSERT(drawText(im1, 4, 10, "ok", fpath, 10) == RES_OK);

    UINT8 vec[20 * 15] = {
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 255, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 255, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 255, 255, 255, 127, 127, 255,
        127, 127, 255, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255,
        127, 127, 127, 255, 127, 255, 127, 255, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 255, 127, 127, 127, 255, 127, 255, 255, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 255, 127, 127,
        127, 255, 127, 255, 127, 255, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 255, 255, 255, 127, 127, 255, 127, 127, 255, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127,
    };

    im2 << vec;
    TEST_ASSERT(im1 == im2);

    if (retVal != RES_OK)
      im1.printSelf(1);

#endif // USE_FREETYPE
  }
};

int main(void)
{
  TestSuite ts;

  ADD_TEST(ts, Test_DrawCircle);
  ADD_TEST(ts, Test_DrawSphere);
#ifdef __linux__
  // The following test uses the DejaVu font through hard-coded
  // Linux paths only
  ADD_TEST(ts, Test_DrawText);
  // TODO: Make it OS-independent?
#endif

  return ts.run();
}
