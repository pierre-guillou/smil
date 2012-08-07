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


#include "DCore.h"
#include "DMorpho.h"
#include "DGui.h"

#include "DImageDraw.hpp"

#include <vector>

class A
{
public:
  void func(const A &a)
  {
    cout << a.val << endl;
  }
  int val;
};

Image_UINT8 create(void)
{
    Image_UINT8 a;
    return a;
}

void outFunc(const Image_UINT8 &a)
{
    cout << a.getPixelCount() << endl;
}

int main(int argc, char *argv[])
{
//     Core::initialize();
//     Gui::getInstance();
    
//     Image_UINT8 im1;
    
    
	Image_UINT8 im1(40000, 1023);
    Image_UINT8 im2(im1);
    imageViewer<UINT8> *viewer;


    drawRectangle(im2, 10, 10, 512, 128);
    viewer = im1.getViewer();
    viewer->drawOverlay(im2);


    // for (int i=0;i<100;i++)
    //   im2 = (im1 & (~im1 + UINT8(100) ));
    // cout << "-------------" << endl;
    // (im1+(unsigned char)100).printSelf();
//     outFunc(create());
//     return 1;
//     if (read("/home/faessel/src/morphee/trunk/utilities/Images/Gray/DNA_small.png", im1)!=RES_OK)
//       read("/home/mat/src/morphee/trunk/utilities/Images/Gray/DNA_small.png", im1);
    
//     if (read("/home/faessel/src/morphee/trunk/utilities/Images/Gray/akiyo_y.png", im1)!=RES_OK)
//       read("/home/mat/src/morphee/trunk/utilities/Images/Gray/akiyo_y.png", im1);
    
//     im1.setSize(1024,1024);

//     im1.setSize(256, 256);
//     
//     for(int j=0;j<im1.getHeight();j++)
//       for(int i=0;i<im1.getWidth();i++)
//       {
// 	  im1.setPixel(i,j, i);
//       }
//     Image_UINT8 im3(im1);
    
    
    im1.show("im1");
    im2.show("im2");
    
//     copy(im2, 256, 0, 0, im1);
    

    
    Gui::execLoop();
    
}
