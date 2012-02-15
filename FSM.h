// * File: FSM.h
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
