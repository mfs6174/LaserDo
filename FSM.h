#ifndef FSM_H
#define FSM_H

#define LDbufsize 60

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
  int iptbuf[LDbufsize];
  CvPoint pntbuf[LDbufsize];
  int bufhd;
  int stsz,hdsz,tttl;
  int cnt;
  bool flag;
};

#endif
