// * File: FSM.cpp 
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
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <vector>
#include <cmath>
#include "FSM.h"
using namespace std;

LDfsm fsm0;
const int sqrrad=400;
const int sttl=80;
const int LDbufsize=40;

inline int gt(int x,int y)
{
  x+=y;
  return ((x%LDbufsize)+LDbufsize)%LDbufsize;
}

inline int dst(CvPoint a,CvPoint b)
{
  return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
}

void ldfsminit(int stsize,int hdsize,int tttl)
{
  fsm0.state=0;
  fsm0.flag=false;
  fsm0.bufhd=0;
  fsm0.stsz=stsize;
  fsm0.hdsz=hdsize;
  fsm0.tttl=tttl;
}
int ldfsmstate()
{
  return fsm0.state;
}

int ldfsmupdate(int ipt,CvPoint pnt)
{
  ipt++;
  if (fsm0.bufhd==LDbufsize-1)
    fsm0.flag=true;
  if ( (++fsm0.bufhd)>=LDbufsize )
    fsm0.bufhd=0;
  int id;
  if (fsm0.flag)
  {
    id=gt(fsm0.bufhd,fsm0.stsz);
    fsm0.iptcnt[fsm0.iptbuf[id]]--;
  }
  fsm0.iptbuf[fsm0.bufhd]=ipt;
  fsm0.pntbuf[fsm0.bufhd]=pnt;
  fsm0.iptcnt[ipt]++;
  int rtsgn=0;
  if (!fsm0.flag)
    return rtsgn;
  switch (fsm0.state)
  {
  case 0:
    if (ipt==2 && fsm0.iptcnt[2]>=(fsm0.stsz*0.8) )
    {
      fsm0.state=1;
      rtsgn=NONE2MOVE;
    }
    //rtsgn==0 do nothing
    break;
  case 1:
    if (ipt==1 && fsm0.iptcnt[1]>=(0.6*fsm0.stsz) )
    {
      fsm0.state=0;
      rtsgn=MOVE2NONE;
    }
    if (ipt==2)
    {
      bool flag=true;
      for (int i=1;i<fsm0.hdsz;i++)
        if (dst(pnt,fsm0.pntbuf[gt(fsm0.bufhd,-i)])>sqrrad)
        {
          flag=false;
          break;
        }
      if (flag)
      {
        rtsgn=MOVE2STATIC;
        fsm0.state=2;
        fsm0.cnt=fsm0.hdsz;
        fsm0.windowpoint=pnt;
      }
    }
    if (!rtsgn)
      rtsgn=KEEPMOVE;
    //do sth for the pnt if it's not N
    break;
  case 2:
    fsm0.cnt++;
    if (ipt==1 && fsm0.iptcnt[1]>=(0.6*fsm0.stsz) )
    {
      fsm0.state=0;
      rtsgn=STATIC2NONE;
    }
   if (ipt==2 && fsm0.iptcnt[2]>=(fsm0.stsz*0.8) )
   {
     bool flag=true;
     for (int i=0;i<fsm0.stsz/2;i++)
        if (dst(fsm0.windowpoint,fsm0.pntbuf[gt(fsm0.bufhd,-i)])<=sqrrad)
        {
          flag=false;
          break;
        }
     if (flag)
     {
       fsm0.state=3;
       fsm0.cnt=0;
       rtsgn=TRACKSTART;
     }
   }
   if (!rtsgn)
   {
     if (fsm0.cnt>sttl)
     {
       rtsgn=STATIC2MOVE;
       fsm0.state=1;
     }
     else
       rtsgn=KEEPSTATIC;
   }
   break;
  case 3:
    fsm0.cnt++;
    if (ipt==1 && fsm0.iptcnt[1]>=(0.6*fsm0.stsz) )
    {
      fsm0.state=0;
      rtsgn=TRACKFINISH;
    }
    if (fsm0.cnt>fsm0.tttl)
    {
      fsm0.state=1;
      rtsgn=TRACKABORT;
    }
    if (!rtsgn)
      rtsgn=KEEPTRACK;
  }
  return rtsgn;
}

