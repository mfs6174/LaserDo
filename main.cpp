#include "cv.h"
#include "highgui.h"
#include <iostream>
#include<cstdlib>
#include<cstdio>
#include<fstream>
#include "mycv.h"
#include "filter.h"
using namespace std;

const double pthold=0.9;
const double redhold=225;

inline void frameprepro(IplImage *fr,IplImage *dst,uchar res[])
{
  int i,j;
  int mm[2]={0,0};
  BwImage dstsh(dst);
  RgbImage frsh(fr);
  for (i=0;i<fr->height;i++)
    for (j=0;j<fr->width;j++)
    {
      int yuv_v=0.615*frsh[i][j].r-0.515*frsh[i][j].g-0.1*frsh[i][j].b;
      // if (yuv_v>mm[0])
      //   mm[0]=yuv_v;
      if (frsh[i][j].r>mm[1])
        mm[1]=frsh[i][j].r;
      dstsh[i][j]=(yuv_v>0)?yuv_v:0;
    }
  for (i=1;i<2;i++)
    if (mm[i]>255)
      res[i]=255;
    else
      res[i]=mm[i];
}

inline void mythreshold_0(IplImage *src,IplImage *dst,IplImage *red,uchar maxv[])
{
  int i,j;
  BwImage dstsh(dst),srcsh(src);
  RgbImage redsh(red);
  double mv[2]={(double)maxv[0],(double)maxv[1]};
  for (i=0;i<src->height;i++)
    for (j=0;j<src->width;j++)
    {
      dstsh[i][j]=0;
      if (redsh[i][j].r/mv[1]>=pthold)
        if ( redsh[i][j].r>=redhold )
          dstsh[i][j]=255;
    }
}

inline CvPoint2D32f getcenterpoint(IplImage *src,int &area)
{
  int i,j,sum=0;
  float xx=0.0,yy=0.0;
  BwImage sh(src);
  area=0;
  for (i=0;i<src->height;i++)
    for (j=0;j<src->width;j++)
    {
      if (sh[i][j]>0)
      {
        area++;
        yy+=sh[i][j]*i;
        xx+=sh[i][j]*j;
        sum+=sh[i][j];
      }
    }
  if (area==0)
    return cvPoint2D32f(0.0,0.0);
  else
    return cvPoint2D32f(xx/sum,yy/sum);
}

int checkstatus(IplImage *src,CvPoint ct,int ars,int rd)
{
  if (ct.x==0&&ct.y==0)
    return 0;
  if (ars>700)
    return -1;
  int ari=0,oct=0;
  BwImage sh(src);
  int i,j;
  for (i=ct.y-rd;i<=ct.y+rd;i++)
    for (j=ct.x-rd;j<=ct.x+rd;j++)
    {
      if (i<0||j<0||i>=src->height||j>=src->width)
        continue;
      ari+=(sh[i][j]&1);
      if( (abs(i-ct.y)==rd || abs(j-ct.x)==rd) && (i!=j) )
        oct+=(sh[i][j]&1);
    }
  if (ari<=15) //面积验证,如果圈内绝对面积较小,可能是噪声杂斑 散布物体点 或者同时有物体和光斑,重心偏离
  {
    //  cout<<ari<<endl;
    return -1;
  }
  if (ari<ars*0.16) //面积比例验证,排除大面积物体(或大量散布物体点),但因为考虑光条,阈值较小
  {
    // cout<<ari<<' '<<ars<<endl;    
    return -1;
  }
  if (oct>=(rd*6.5)) //周长验证,排除非散布的对称形状小面积物体(光条细长)
    return -1;
  return 1;
}

int main( int argc, char** argv )
{
  	int fps=25; //捕捉帧率
    int Vhold=27;
    int chkrad=7;
    int ar=0;
    int sta=0;
	CvCapture* capture = 0;
	IplImage* frame = 0;
    uchar maxv[2]={0,0};
	capture = cvCaptureFromCAM(1);
	cvNamedWindow( "cam", 1);
    cvMoveWindow("cam",100,100);
    //cvNamedWindow( "threshold", 1);
    //cvMoveWindow("threhold",800,100);
    if (!cvGrabFrame(capture))
    {
      cout<<"can not capture"<<endl;
      exit(0);
    }
    frame=cvQueryFrame(capture);
    fps=(int)cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);
    IplImage *pframe=cvCreateImage(cvSize(370,277),frame->depth,3);
    IplImage *Vv=cvCreateImage(cvSize(370,277),frame->depth,1);
    CvMat *wp0=cvCreateMat(277, 370, CV_32FC1);
    CvMat *wp3=cvCreateMat(277, 370, CV_32FC1);
    IplImage *wp1=cvCreateImage(cvSize(370,277),frame->depth,1);
    CvMat *bg=cvCreateMat(277, 370, CV_32FC1);
    CvMat *ramask=cvCreateMat(277, 370, CV_8UC1);
    IplImage *fiimg=cvCreateImage(cvSize(370,277),frame->depth,1);
    IplImage *wp2=cvCreateImage(cvSize(370,277),frame->depth,1);

    IplConvKernel *cls=cvCreateStructuringElementEx(2,2,1,1,CV_SHAPE_RECT,0);
    CvPoint2D32f lp;
    CvPoint pp;
    LDFilter flt;
    ldfilterinit(&flt,277,370);
    cvResize(frame,pframe);
    frameprepro(pframe,Vv,maxv);
    cvConvert(Vv,bg);
    int cnt=0;
	for(;;) //一直读
	{
		frame = cvQueryFrame( capture );
        // cnt++;
        // if (cnt%18==0)
        // {
        //   fps=(int)cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);
        //   cout<<fps<<endl;
        //   cnt=0;
        // }
        //因为不支持,关闭fps获取
        cvResize(frame,pframe);
        frameprepro(pframe,Vv,maxv);
        cvSmooth(Vv,Vv,CV_GAUSSIAN);
        cvConvert(Vv,wp3);
        cvAbsDiff(wp3,bg,wp0);
        cvThreshold(wp0, wp2, Vhold, 255, CV_THRESH_BINARY);
        cvDilate(wp2,wp2,cls,1);
        mythreshold_0(Vv,wp1,pframe,maxv);
        cvAnd(wp2,wp1,fiimg);
        cvDilate(fiimg,fiimg,0,1);
        lp=getcenterpoint(fiimg,ar);
        sta=checkstatus(fiimg,cvPointFrom32f(lp),ar,chkrad);
        CvScalar dcl;
        if (sta<=0)
        {
          dcl=CV_RGB(255,0,0);
          cvRunningAvg(wp3,bg,0.18,0);

        }
        else
        {
          dcl=CV_RGB(0,255,0);
          cvNot(fiimg,ramask);
          cvRunningAvg(wp3, bg, 0.09, ramask);
        }
        if (sta)
        {
          ldfilterupdate(&flt,lp);
          if (sta>0)
          {
            //for (int i=1;i<=100;i++)
              ldfilterupdate(&flt,lp);
          }
          cvCircle(pframe,cvPointFrom32f(lp),6,dcl,2);
        }
        pp=ldfilterpredict(&flt);
        cvCircle(pframe,pp,6,CV_RGB(0,0,255),2);
		cvShowImage( "cam", pframe ); //显示一帧图像
        //cvShowImage("threshold",fiimg);
		if (cvWaitKey(1)>=0)//视频速度
          break;
	}
	cvReleaseCapture(&capture);
    cvReleaseImage(&Vv);
    cvReleaseImage(&wp1);
    cvReleaseImage(&fiimg);
    cvReleaseImage(&wp2);
    cvReleaseImage(&pframe);
    cvReleaseMat(&wp0);
    cvReleaseMat(&bg);
    cvReleaseMat(&ramask);
    cvReleaseStructuringElement(&cls);
    ldfilterrelease(&flt);
    cvDestroyAllWindows();
   	return 0;
}
