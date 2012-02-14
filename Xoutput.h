#ifndef XOUTPUT_H
#define XOUTPUT_H

struct Ldlocater
{
  double a,b,c,d;
};

void ldxoinit();
void ldxorelease();
void ldxomouseslide(CvPoint now,double factor);
void ldxomousereset();
void ldxoclick(CvPoint pnt);
void ldxolocateinit(CvCapture * cap);
void ldxomouselocate(CvPoint ipnt,double para1=0.0);

#endif
