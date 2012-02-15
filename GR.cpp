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

