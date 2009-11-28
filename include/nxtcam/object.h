/*
    object.h
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

#ifndef _NXTCAM_OBJECT_H_
#define _NXTCAM_OBJECT_H_

#define NXTCAM_OBJECT_IS_OVERSIZED(obj) \
          ((obj)->w>=NXTCAM_FRAME_WIDTH || (obj)->h>=NXTCAM_FRAME_HEIGHT)

typedef struct {
  int id;
  int color;
  int x,y;
  int w,h;
  int x2,y2;
} nxtcam_object_t;

#endif /* _NXTCAM_OBJECT_H_ */
