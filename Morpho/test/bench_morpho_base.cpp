/*
 * Copyright (c) 2011-2015, Matthieu FAESSEL and ARMINES
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



#include "Core/include/DCore.h"
#include "DMorpho.h"

using namespace smil;

int main()
{
    Image<UINT8> im1(5562, 7949);
//    Image<UINT8> im1(1024, 1024);
    Image<UINT8> im2(im1);
    
    StrElt generic_sSE(sSE());
    generic_sSE.seT = SE_Generic;
    
    UINT BENCH_NRUNS = 1E2;
    
    BENCH_IMG_STR(dilate, "hSE", im1, im2, hSE());
    BENCH_IMG_STR(dilate, "sSE", im1, im2, sSE());
    BENCH_IMG_STR(dilate, "generic sSE", im1, im2, generic_sSE());
    BENCH_IMG_STR(dilate, "CrossSE", im1, im2, CrossSE());
    BENCH_IMG_STR(open, "hSE", im1, im2, hSE());
    BENCH_IMG_STR(open, "sSE", im1, im2, sSE());
    BENCH_IMG_STR(open, "CrossSE", im1, im2, CrossSE());
    
    cout << endl;
    
    // 3D
    
    im1.setSize(500, 500, 100);
    im2.setSize(im1);
        
    BENCH_IMG_STR(dilate, "CubeSE", im1, im2, CubeSE());
    BENCH_IMG_STR(dilate, "Cross3DSE", im1, im2, Cross3DSE());
    BENCH_IMG_STR(open, "CubeSE", im1, im2, CubeSE());
    BENCH_IMG_STR(open, "Cross3DSE", im1, im2, Cross3DSE());
    BENCH_IMG_STR(open, "RhombicuboctahedronSE", im1, im2, RhombicuboctahedronSE());
}

