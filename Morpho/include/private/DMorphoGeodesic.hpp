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


#ifndef _D_MORPHO_GEODESIC_HPP
#define _D_MORPHO_GEODESIC_HPP

#include "DLineArith.hpp"
#include "DMorphImageOperations.hpp"
#include "DImageArith.hpp"
#include "DMorphoHierarQ.hpp"


// Geodesy

template <class T>
RES_T geoDil(Image<T> &imIn, Image<T> &imMask, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    StrElt tmpSe(se);
    tmpSe.size = 1;
    
    RES_T res = inf(imIn, imMask, imOut);
    
    for (int i=0;i<se.size;i++)
    {
	res = dilate<T>(imOut, imOut, tmpSe);
	if (res==RES_OK)
	  res = inf(imOut, imMask, imOut);
	if (res!=RES_OK)
	  break;
    }
    return res;
}

template <class T>
RES_T geoEro(Image<T> &imIn, Image<T> &imMask, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    StrElt tmpSe(se);
    tmpSe.size = 1;
    
    RES_T res = sup(imIn, imMask, imOut);
    
    for (int i=0;i<se.size;i++)
    {
	res = erode(imOut, imOut, tmpSe);
	if (res==RES_OK)
	  res = sup(imOut, imMask, imOut);
	if (res!=RES_OK)
	  break;
    }
    return res;
}

template <class T>
RES_T geoBuild(Image<T> &imIn, Image<T> &imMask, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    StrElt tmpSe(se);
    tmpSe.size = 1;
    
    RES_T res = inf(imIn, imMask, imOut);
    bool updatesEnabled = imOut.updatesEnabled;
    imOut.updatesEnabled = false;
    
    int vol1 = vol(imOut), vol2;
    while (true)
    {
	res = dilate<T>(imOut, imOut, tmpSe);
	if (res==RES_OK)
	  res = inf(imOut, imMask, imOut);
	if (res!=RES_OK)
	  break;
	vol2 = vol(imOut);
	if (vol2==vol1)
	  break;
	vol1 = vol2;
    }
    imOut.updatesEnabled = updatesEnabled;
    imOut.modified();
    return res;
}

template <class T>
RES_T geoDualBuild(Image<T> &imIn, Image<T> &imMask, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    StrElt tmpSe(se);
    tmpSe.size = 1;
    
    RES_T res = sup(imIn, imMask, imOut);
    bool updatesEnabled = imOut.updatesEnabled;
    imOut.updatesEnabled = false;
    
    int vol1 = vol(imOut), vol2;
    
    while (true)
    {
	res = erode(imOut, imOut, tmpSe);
	if (res==RES_OK)
	  res = sup(imOut, imMask, imOut);
	if (res!=RES_OK)
	  break;
	vol2 = vol(imOut);
	if (vol2==vol1)
	  break;
	vol1 = vol2;
    }
    imOut.updatesEnabled = updatesEnabled;
    imOut.modified();
    return res;
}






template <class T, class HQcompT>
RES_T initBuildHierarchicalQueue(Image<T> &imIn, HierarchicalQueue<T, HQcompT> &hq)
{
    // Empty the priority queue
    hq.reset();
    
    typename ImDtTypes<T>::lineType inPixels = imIn.getPixels();
    
    UINT x, y, z;
    UINT s[3];
    
    imIn.getSize(s);
    UINT offset = 0;
    
    for (UINT i=0;i<imIn.getPixelCount();i++)
    {
	hq.push(*inPixels, offset);
	inPixels++;
	offset++;
    }
    
//     hq.printSelf();
    return RES_OK;
}



template <class T, class operatorT, class HQcompT>
RES_T processBuildHierarchicalQueue(Image<T> &imIn, Image<T> &imMark, Image<UINT8> &imStatus, HierarchicalQueue<T, HQcompT> &hq, StrElt &se)
{
    typename ImDtTypes<T>::lineType inPixels = imIn.getPixels();
    typename ImDtTypes<T>::lineType markPixels = imMark.getPixels();
    typename ImDtTypes<UINT8>::lineType statPixels = imStatus.getPixels();
    
    vector<int> dOffsets;
    operatorT oper;
    
    vector<Point>::iterator it_start = se.points.begin();
    vector<Point>::iterator it_end = se.points.end();
    vector<Point>::iterator it;
    
    vector<UINT> tmpOffsets;
    
    UINT s[3];
    imIn.getSize(s);
    
    // set an offset distance for each se point
    for(it=it_start;it!=it_end;it++)
    {
	dOffsets.push_back(it->x - it->y*s[0] + it->z*s[0]*s[1]);
    }
    
    vector<int>::iterator it_off_start = dOffsets.begin();
    vector<int>::iterator it_off;
    
    
    while(!hq.empty())
    {
	
	HQToken<T> token = hq.top();
	hq.pop();
	UINT x0, y0, z0;
	
	UINT curOffset = token.offset;
	
	// Give the point the label "FINAL" in the status image
	statPixels[curOffset] = HQ_FINAL;
	
	imIn.getCoordsFromOffset(curOffset, x0, y0, z0);
	
	int x, y, z;
	UINT nbOffset;
	UINT8 nbStat;
	
	int oddLine = se.odd * y0%2;
	
	for(it=it_start,it_off=it_off_start;it!=it_end;it++,it_off++)
	    if (it->x!=0 || it->y!=0 || it->z!=0) // useless if x=0 & y=0 & z=0
	{
	    
	    x = x0 + it->x;
	    y = y0 - it->y;
	    z = z0 + it->z;
	    
	    if (oddLine)
	      x += (y+1)%2;
	  
	    if (x>=0 && x<s[0] && y>=0 && y<s[1] && z>=0 && z<s[2])
	    {
		nbOffset = curOffset + *it_off;
		
		if (oddLine)
		  nbOffset += (y+1)%2;
		
		nbStat = statPixels[nbOffset];
		
		if (nbStat==HQ_CANDIDATE)
		{
		    inPixels[nbOffset] = oper(inPixels[curOffset], markPixels[nbOffset]);
		    statPixels[nbOffset] = HQ_QUEUED;
		    hq.push(inPixels[nbOffset], nbOffset);
		}
		
	    }
	}

    }
    return RES_OK;    
}

template <class T>
struct minFunctor 
{
  inline int operator()(T a, T b) { return min(a, b); }
};

template <class T>
struct maxFunctor 
{
  inline int operator()(T a, T b) { return max(a, b); }
};

/**
 * Dual reconstruction (using hierarchical queues).
 */
template <class T>
RES_T dualBuild(Image<T> &imIn, Image<T> &imMark, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    if (!areAllocated(&imIn, &imMark, &imOut, NULL))
      return RES_ERR_BAD_ALLOCATION;
    
    if (!haveSameSize(&imIn, &imMark, &imOut, NULL))
      return RES_ERR_BAD_SIZE;
    
    SLEEP(imOut);
    
    Image<UINT8> imStatus(imIn);
    HierarchicalQueue<T> pq;
    
    // Make sure that imIn >= imMark
    sup(imIn, imMark, imOut);
    
    // Set all pixels in the status image to CANDIDATE
    fill(imStatus, (UINT8)HQ_CANDIDATE);
    
    // Initialize the PQ
    initBuildHierarchicalQueue(imOut, pq);
    processBuildHierarchicalQueue<T, maxFunctor<T> >(imOut, imMark, imStatus, pq, se);
    
    WAKE_UP(imOut);
    imOut.modified();
    
    return RES_OK;
}

/**
 * Reconstruction (using hierarchical queues).
 */
template <class T>
RES_T build(Image<T> &imIn, Image<T> &imMark, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    if (!areAllocated(&imIn, &imMark, &imOut, NULL))
      return RES_ERR_BAD_ALLOCATION;
    
    if (!haveSameSize(&imIn, &imMark, &imOut, NULL))
      return RES_ERR_BAD_SIZE;
    
    SLEEP(imOut);
    
    Image<UINT8> imStatus(imIn);
    
    // Reverse hierarchical queue (the highest token correspond to the highest gray value)
    typedef typename std::less< HQToken<T> > compareType;
    HierarchicalQueue<T, compareType > rpq;
    
    // Make sure that imIn <= imMark
    inf(imIn, imMark, imOut);
    
    // Set all pixels in the status image to CANDIDATE
    fill(imStatus, (UINT8)HQ_CANDIDATE);
    
    // Initialize the PQ
    initBuildHierarchicalQueue(imOut, rpq);
    processBuildHierarchicalQueue<T, minFunctor<T> >(imOut, imMark, imStatus, rpq, se);
    
    WAKE_UP(imOut);
    imOut.modified();
    
    return RES_OK;
}





template <class T>
RES_T fillHoles(Image<T> &imIn, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    if (!areAllocated(&imIn, &imOut, NULL))
      return RES_ERR_BAD_ALLOCATION;
    
    if (!haveSameSize(&imIn, &imOut, NULL))
      return RES_ERR_BAD_SIZE;
    
    StrElt tmpSe(se);
    tmpSe.size = 1;
    
    RES_T res;
    
    Image<T> tmpIm(imIn);
    
    fill(tmpIm, numeric_limits<T>::max());
    dualBuild(tmpIm, imIn, imOut);
    
    return res;
}

template <class T>
RES_T levelPics(Image<T> &imIn, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    if (!areAllocated(&imIn, &imOut, NULL))
      return RES_ERR_BAD_ALLOCATION;
    
    if (!haveSameSize(&imIn, &imOut, NULL))
      return RES_ERR_BAD_SIZE;
    
    Image<T> tmpIm(imIn);
    inv(imIn, tmpIm);
    fillHoles(tmpIm, imOut);
    inv(imOut, imOut);
    
//     return res;
    return RES_OK;
}


/**
 * Ugly temporary distance function
 */
template <class T>
RES_T dist(Image<T> &imIn, Image<T> &imOut, StrElt se=DEFAULT_SE())
{
    Image<T> tmpIm(imIn);
    
    // Set image to 1 when pixels are !=0
    inf(imIn, T(1), tmpIm);
    
    copy(tmpIm, imOut);
    
    do
    {
	erode(tmpIm, tmpIm, se);
	add(tmpIm, imOut, imOut);
	
    } while (vol(tmpIm)!=0);

    imOut.modified();
    return RES_OK;
}



#endif // _D_MORPHO_GEODESIC_HPP

