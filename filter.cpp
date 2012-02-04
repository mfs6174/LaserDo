#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "filter.h"
using namespace std;

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
