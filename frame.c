/*
    frame.c
    aNXTCam - a NXTCam configuration tool
    Graphical tool for Mindsensors NXTCam (V2.0)
    Copyright (C) 2009  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "nxtcam/frame.h"
#include "nxtcam/color.h"
#include "nxtcam/log.h"

static int _mean_correlation(int a1,int a2,int b1,int b2) {
  int da,db;

  da = abs(a1-a2);
  db = abs(b1-b2);

  if (da<db) {
    return (a1+a2)/2;
  }
  else if (db<da) {
    return (b1+b2)/2;
  }
  else /* (db==da) */ {
    return (a1+a2+b1+b2)/4;
  }
}

void nxtcam_frame_bayer_interpolate(nxtcam_frame_t frame) {
  int x,y;
  nxtcam_rgb_t *p;

  // interpolate everything except edges and borders
  for (y=0;y<NXTCAM_FRAME_HEIGHT;y++) {
    for (x=0;x<NXTCAM_FRAME_WIDTH;x++) {
      p = nxtcam_frame_pixel(frame,x,y);

      switch (2*(y%2)+(x%2)) {
        case 0:
          // Green pixel
          // Red is interpolated horizontally
          // Blue is interpolated vertically
          p->r = (nxtcam_frame_r(frame,x-1,y)+nxtcam_frame_r(frame,x+1,y))/2;
          p->b = (nxtcam_frame_b(frame,x,y+1)+nxtcam_frame_b(frame,x,y+1))/2;
          break;
        case 1:
          // Red pixel
          // Green is interpolated horizontally or vertically
          // Blue is interpolated diagonal
          p->g = _mean_correlation(
                   nxtcam_frame_g(frame,x-1,y),
                   nxtcam_frame_g(frame,x+1,y),
                   nxtcam_frame_g(frame,x,y-1),
                   nxtcam_frame_g(frame,x,y+1)
                 );
          p->b = _mean_correlation(
                   nxtcam_frame_b(frame,x-1,y-1),
                   nxtcam_frame_b(frame,x+1,y-1),
                   nxtcam_frame_b(frame,x-1,y+1),
                   nxtcam_frame_b(frame,x+1,y+1)
                 );
          break;
        case 2:
          // Blue pixel
          // Green is interpolated horizontally or vertically
          // Red is interpolated diagonal
          p->g = _mean_correlation(
                   nxtcam_frame_g(frame,x-1,y),
                   nxtcam_frame_g(frame,x+1,y),
                   nxtcam_frame_g(frame,x,y-1),
                   nxtcam_frame_g(frame,x,y+1)
                 );
          p->r = _mean_correlation(
                   nxtcam_frame_r(frame,x-1,y-1),
                   nxtcam_frame_r(frame,x+1,y-1),
                   nxtcam_frame_r(frame,x-1,y+1),
                   nxtcam_frame_r(frame,x+1,y+1)
                 );
          break;
        case 3:
          // Green pixel
          // Blue is interpolated horizontally
          // Red is interpolated vertically
          p->b = (nxtcam_frame_b(frame,x-1,y)+nxtcam_frame_b(frame,x+1,y))/2;
          p->r = (nxtcam_frame_r(frame,x,y+1)+nxtcam_frame_r(frame,x,y+1))/2;
          break;
      }
    }
  }
}

int nxtcam_frame_dump(const char *filename,nxtcam_frame_t frame) {
  FILE *fd = fopen(filename,"w");
  if (fd!=NULL) {
    logmsg("Dumping frame into file: %s\n",filename);
    fprintf(fd,"P6\n%d %d %d\n",NXTCAM_FRAME_WIDTH,NXTCAM_FRAME_HEIGHT,0xFF);
    fwrite(frame,1,NXTCAM_FRAMESIZE,fd);
    fclose(fd);
    return 0;
  }
  else {
    return -1;
  }
}

uint8_t nxtcam_frame_channel(nxtcam_frame_t frame,int x,int y,int channel) {
  nxtcam_rgb_t *p;

  if (x<0 || x>=NXTCAM_FRAME_WIDTH || y<0 || y>=NXTCAM_FRAME_HEIGHT) {
    return 0x80;
  }

  p = nxtcam_frame_pixel(frame,x,y);

  switch (channel) {
    case NXTCAM_RGB_R:
      return p->r;
    case NXTCAM_RGB_G:
      return p->g;
    case NXTCAM_RGB_B:
      return p->b;
    default:
      return 0;
  }
}
