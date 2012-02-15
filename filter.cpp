// * File: filter.cpp 
// * Author: Zhang Xijin(mfs6174)
// * Email: mfs6174@gmail.com 
// * Copyright (c) 2012 Zhang Xijin(mfs6174@gmail.com).  All rights reserved.
// *
// * Redistribution and use in source and binary forms, with or without
// * modification, are permitted provided that the following conditions
// * are met:
// * 1. Redistributions of source code must retain the above copyright
// *    notice, this list of conditions and the following disclaimer.
// * 2. Redistributions in binary form must reproduce the above copyright
// *    notice, this list of conditions and the following disclaimer in the
// *    documentation and/or other materials provided with the distribution.
// * 3. All advertising materials mentioning features or use of this software
// *    must display the following acknowledgement:
// *      This product includes software developed by mfs6174(mfs6174@gmail.com).
// * 4. Neither the name of the Software nor the names of its contributors
// *    may be used to endorse or promote products derived from this software
// *    without specific prior written permission.
// *
// * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "filter.h"
using namespace std;

//部分代码来自 onezeros 的代码(http://blog.csdn.net/onezeros/article/details/6318944) Thanks
const int stateNum=6;
const int measureNum=2;
const float A[stateNum][stateNum] ={//transition matrix
    1,0,1,0,0,0,
    0,1,0,1,0,0,
    0,0,1,0,1,0,
    0,0,0,1,0,1,
    0,0,0,0,1,0,
    0,0,0,0,0,1,
};
void ldfilterinit(LDFilter *it,int winHeight,int winWidth)
{
  it->height=winHeight;
  it->width=winWidth;
  it->kalman=cvCreateKalman( stateNum, measureNum, 0 );//state(x,y,detaX,detaY)
  it->process_noise = cvCreateMat( stateNum, 1, CV_32FC1 );
  it->measurement = cvCreateMat( measureNum, 1, CV_32FC1 );//measurement(x,y)
  it->rng = cvRNG(-1);
  memcpy( it->kalman->transition_matrix->data.fl,A,sizeof(A));
  cvSetIdentity(it->kalman->measurement_matrix,cvRealScalar(1) );
  cvSetIdentity(it->kalman->process_noise_cov,cvRealScalar(1e-5));
  cvSetIdentity(it->kalman->measurement_noise_cov,cvRealScalar(1e-1));
  cvSetIdentity(it->kalman->error_cov_post,cvRealScalar(1));
  //initialize post state of kalman filter at random
  cvRandArr(&(it->rng),it->kalman->state_post,CV_RAND_UNI,cvRealScalar(0),cvRealScalar(winHeight>winWidth?winWidth:winHeight));
}

CvPoint ldfilterpredict(LDFilter *it)
{
  const CvMat* prediction=cvKalmanPredict(it->kalman,0);
  CvPoint predict_pt=cvPoint((int)(prediction->data.fl[0]),(int)(prediction->data.fl[1]));
  if (predict_pt.x<0)
    predict_pt.x=0;
  else
    if (predict_pt.x>it->width)
      predict_pt.x=it->width;
  if (predict_pt.y<0)
    predict_pt.y=0;
  else
    if (predict_pt.y>it->height)
      predict_pt.y=it->height;
  return predict_pt;
}

void ldfilterupdate(LDFilter *it,const CvPoint2D32f &pnt)
{
  //3.update measurement
  it->measurement->data.fl[0]=(float)pnt.x;
  it->measurement->data.fl[1]=(float)pnt.y;
  //4.update
  cvKalmanCorrect(it->kalman,it->measurement);
}

void ldfilterrelease(LDFilter *it)
{
  cvReleaseKalman(&(it->kalman));
  cvReleaseMat(&(it->process_noise));
  cvReleaseMat(&(it->measurement));
}
