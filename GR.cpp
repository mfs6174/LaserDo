// * File: GR.cpp 
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
#include "ml.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>
#include "GR.h"
using namespace std;

const int veclong=8;
inline int sqdis(CvPoint a,CvPoint b)
{
  return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
}

int ldgrinput(vector<CvPoint> &ipt)
{
  vector<CvPoint> op(ipt);
  int i;
  //cout<<op.size()<<endl;
  //平滑轨迹到包括veclong个向量
  while (op.size()>veclong+1)
  {
    int pos,mm=0x7FFFFFFF;
    for (i=0;i<op.size()-1;i++)
      if (sqdis(op[i],op[i+1])<mm)
      {
        mm=sqdis(op[i],op[i+1]);
        pos=i;
      }
    op[pos].x=(op[pos].x+op[pos+1].x)/2;
    op[pos].y=(op[pos].y+op[pos+1].y)/2;
    op.erase(op.begin()+(pos+1));
  }
  if ( op[op.size()-1].x>op[0].x )
    return 1;
  //传输给SVM进行识别,返回识别结果标志
  return 0;
}

