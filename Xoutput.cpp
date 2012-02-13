/* ref: http://www.ishiboo.com/~danny/Projects/xwarppointer/ */
#include "cv.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "Xoutput.h"

using namespace std;

Display *display;
Window root;
int screen;
int winwidth;
int winheight;

CvPoint lastp;

//初始化
void ldxoinit()
{
  if ((display = XOpenDisplay(NULL)) == NULL) {
    fprintf(stderr, "Cannot open local X-display.\n");
    return;
  }
  root=DefaultRootWindow(display);
  screen=DefaultScreen(display);
  winwidth=DisplayWidth(display,screen);
  winheight=DisplayHeight(display,screen);
  lastp.x=lastp.y=-1;
}

void ldxomousereset()
{
  lastp.x=lastp.y=-1;
}

void ldxorelease()
{
  XCloseDisplay(display);
}

//得到坐标
inline void GetCursorPos(int &x,int &y)
{
  int tmp;unsigned int tmp2;
  Window fromroot, tmpwin;
  XQueryPointer(display, root, &fromroot, &tmpwin, &x, &y, &tmp, &tmp, &tmp2);
}
//设置坐标
void SetCursorPos(int x,int y)
{
  int tmp;
  XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
  XFlush(display);
}
 
//模拟点击 modified from
/* http://www.linuxquestions.org/questions/programming-9/simulating-a-mouse-click-594576/ */
int mouseClick(int button,int x,int y)
{
  XEvent event;
  int rtr=0;
  memset(&event, 0x00, sizeof(event));
  event.type = ButtonPress;
  event.xbutton.button = button;
  event.xbutton.same_screen = True;
  if (!(x<0))
    SetCursorPos(x,y);
  XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
  event.xbutton.subwindow = event.xbutton.window;
  while(event.xbutton.subwindow)
  {
    event.xbutton.window = event.xbutton.subwindow;
    XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
  }
  if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
    rtr=-1;
  XFlush(display);
  usleep(100000);
  event.type = ButtonRelease;
  event.xbutton.state = 0x100;
  if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
    rtr=-1;
  XFlush(display);
  return rtr;
}

void ldxomouseslide(CvPoint now,double factor)
{
  if (lastp.x<0)
  {
    lastp=now;
    return;
  }
  double dx=now.x-lastp.x,dy=now.y-lastp.y;
  dx*=factor;dy*=factor;
  lastp=now;
  //  printf("%.3lf %.3lf\n",dx,dy);
  int x,y;
  GetCursorPos(x,y);
  x+=(int)dx;
  y+=(int)dy;
  if (x<0)
    x=0;
  if (y<0)
    y=0;
  if (x>winwidth)
    x=winwidth;
  if (y>winheight)
    y=winheight;
  SetCursorPos(x,y);
  return;
}

void ldxoclick(CvPoint pnt)
{
  if (pnt.x<0)
  {
    mouseClick(Button1,pnt.x,pnt.y);
  }
  else
  {
  }
}

void ldxomouselocate(CvPoint ipnt,double para1)
{
}
