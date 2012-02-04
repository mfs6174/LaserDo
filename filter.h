#ifndef FILTER_H
#define FILTER_H
struct LDFilter
{
  CvKalman* kalman;
  CvMat* process_noise;
  CvMat* measurement;
  CvRNG rng;
  int width,height;
};

void ldfilterinit(LDFilter *it,int winHeight,int winWidth);
CvPoint ldfilterpredict(LDFilter *it);
void ldfilterupdate(LDFilter *it,const CvPoint2D32f &pnt);
void ldfilterrelease(LDFilter *it);

#endif
