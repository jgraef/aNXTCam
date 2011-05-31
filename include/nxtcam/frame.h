/*
    device.h
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

#ifndef _NXTCAM_FRAME_H_
#define _NXTCAM_FRAME_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nxtcam/color.h"

#define NXTCAM_FRAME_WIDTH  176
#define NXTCAM_FRAME_HEIGHT 144

#define NXTCAM_FRAMESIZE (sizeof(nxtcam_rgb_t)*NXTCAM_FRAME_WIDTH*NXTCAM_FRAME_HEIGHT)

#define nxtcam_frame_r(frame,x,y) nxtcam_frame_channel(frame,x,y,NXTCAM_RGB_R)
#define nxtcam_frame_g(frame,x,y) nxtcam_frame_channel(frame,x,y,NXTCAM_RGB_G)
#define nxtcam_frame_b(frame,x,y) nxtcam_frame_channel(frame,x,y,NXTCAM_RGB_B)

typedef nxtcam_rgb_t *nxtcam_frame_t;

uint8_t nxtcam_frame_channel(nxtcam_frame_t frame,int x,int y,int channel);

static __inline__ nxtcam_rgb_t *nxtcam_frame_pixel(nxtcam_frame_t frame,int x,int y) {
  return frame+y*NXTCAM_FRAME_WIDTH+x;
}

static __inline__ nxtcam_frame_t nxtcam_frame_clear(nxtcam_frame_t frame) {
  memset(frame,0,NXTCAM_FRAMESIZE);
  return frame;
}

static __inline__ nxtcam_frame_t nxtcam_frame_copy(nxtcam_frame_t dst,nxtcam_frame_t src) {
  memcpy(dst,src,NXTCAM_FRAMESIZE);
  return dst;
}

static __inline__  nxtcam_frame_t nxtcam_frame_alloc(void) {
  return nxtcam_frame_clear(malloc(NXTCAM_FRAMESIZE));
}

static __inline__ void nxtcam_frame_free(nxtcam_frame_t frame) {
  free(frame);
}
	//Create the RGB Pixel Arrays that store the values for the entire captured image
	int pixelArrayR[NXTCAM_FRAME_WIDTH][NXTCAM_FRAME_HEIGHT];
	int pixelArrayG[NXTCAM_FRAME_WIDTH][NXTCAM_FRAME_HEIGHT];
	int pixelArrayB[NXTCAM_FRAME_WIDTH][NXTCAM_FRAME_HEIGHT];

#endif /* _NXTCAM_FRAME_H_ */
