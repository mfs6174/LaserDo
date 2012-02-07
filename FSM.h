#ifndef FSM_H
#define FSM_H

#define MAXbufsize 90

#define NONE2MOVE 1
#define MOVE2NONE 2
#define MOVE2STATIC 3
#define KEEPMOVE 4
#define STATIC2NONE 5
#define KEEPSTATIC 6
#define STATIC2MOVE 7
#define TRACKSTART 8
#define TRACKABORT 9
#define TRACKFINISH 10
#define KEEPTRACK 11

struct LDfsm
{
  int state;
  CvPoint windowpoint;
  int iptcnt[3];
  int iptbuf[MAXbufsize];
  CvPoint pntbuf[MAXbufsize];
  int bufhd;
  int stsz,hdsz,tttl;
  int cnt;
  bool flag;
};

void ldfsminit(int stsize,int hdsize,int tttl);
int ldfsmupdate(int ipt,CvPoint pnt);
int ldfsmstate();
#endif
