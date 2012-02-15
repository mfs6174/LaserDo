// * File: main.cpp 
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

#include "cv.h"
#include "highgui.h"
#include <iostream>
#include<cstdlib>
#include<cstdio>
#include<fstream>
#include<vector>

#include "mycv.h"
#include "filter.h"
#include "FSM.h"
#include "Xoutput.h"
#include "GR.h"
using namespace std;

const double pthold=0.9;
const double redhold=215;
const double slidefactor=3.0;
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

void draw1trace(const vector<CvPoint> &rec,IplImage *dst)
{
  int i;
  CvScalar color=CV_RGB(0,0,255);
  for (i=0;i<rec.size()-1;i++)
    cvLine(dst,rec[i],rec[i+1],color,2);
}

void ldprocesstrace(vector<CvPoint> &rec,bool &flag0)
{
  int rtr;
  flag0=true;
  rtr=ldgrinput(rec);
  if (rtr==1)
    ldxodoubleclick(cvPoint(-1,-1));
}

int main( int argc, char** argv )
{
  	int fps=25; //捕捉帧率
    bool sldorpnt=true;
    bool draw_mode=false;
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
    //fps=(int)cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);
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
    CvPoint pp,ilp;
    LDFilter flt;
    CvFont font;
    string sstate[4]={"NONE","MOVING","STATIC","TRACKING"};
	cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,1,1);

    vector<CvPoint> tracerec;
    vector< vector<CvPoint> > drawstore;
    drawstore.clear();
    if (!sldorpnt)
    {
      ldxolocateinit(capture);
    }
    ldfilterinit(&flt,277,370);
    ldfsminit(5,16,50);
    ldxoinit();
    cvResize(frame,pframe);
    frameprepro(pframe,Vv,maxv);
    cvConvert(Vv,bg);
    int cnt=0;
    bool nd2draw=false;
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
        ilp=cvPointFrom32f(lp);
        if (sta)
        {
          ldfilterupdate(&flt,lp);
          if (sta>0)
          {
              ldfilterupdate(&flt,lp);
          }
          cvCircle(pframe,ilp,6,dcl,2);
        }
        pp=ldfilterpredict(&flt);
        int rtr=ldfsmupdate(sta,ilp);
        switch (rtr)
        {
        case NONE2MOVE:
          nd2draw=false;
          if ( (!tracerec.empty()) && draw_mode)
            drawstore.push_back(tracerec);
          if (sldorpnt)
          {
            ldxomouseslide(ilp,slidefactor);
          }
          else
          {
            ldxomouselocate(ilp);
          }
          break;
        case STATIC2MOVE:
          if (sldorpnt)
          {
            ldxomouseslide(ilp,slidefactor);
          }
          else
          {
            ldxomouselocate(ilp);
          }
          tracerec.clear();
          break;
        case MOVE2NONE:
          ldxomousereset();
          break;
        case MOVE2STATIC:
          tracerec.clear();
          tracerec.push_back(pp);
          break;
        case KEEPSTATIC:
          tracerec.push_back(pp);
          break;
        case STATIC2NONE:
          tracerec.clear();
          ldxomousereset();
          ldxoclick(cvPoint(-1,-1));
          break;
        case KEEPMOVE:
          if (sta>0)
            if (sldorpnt)
            {
              ldxomouseslide(ilp,slidefactor);
            }
            else
            {
              ldxomouselocate(ilp);
            }
          break;
        case TRACKSTART:
          ldxomousereset();
          nd2draw=true;
          // tracerec.clear();
          // tracerec.push_back(pp);
          //改到进入static就提前开始记录
          break;
        case TRACKABORT:
          nd2draw=false;
          tracerec.clear();
          break;
        case TRACKFINISH:
          // for (int i=1;i<=2;i++)
          //   tracerec.pop_back();
          // tracerec.push_back(ilp);
          ldprocesstrace(tracerec,nd2draw);
          break;
        case KEEPTRACK:
          tracerec.push_back(ilp);//在没有搞定kalman滤波之前还是用真实轨迹吧
          break;
        }
        if (draw_mode)
          for (int i=0;i<drawstore.size();i++)
            draw1trace(drawstore[i],pframe);
        if (nd2draw)
          draw1trace(tracerec,pframe);
        int fsmstate=ldfsmstate();
        cvPutText(pframe,(sstate[fsmstate]).c_str(),cvPoint(20,30),&font,CV_RGB(0,0,255));
        cvShowImage( "cam", pframe ); //显示一帧图像
		if (cvWaitKey(1)>=0)
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
    ldxorelease();
    cvDestroyAllWindows();
   	return 0;
}
