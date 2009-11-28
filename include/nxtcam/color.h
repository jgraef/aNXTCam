/*
    color.h
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
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

#ifndef _NXTCAM_COLOR_H_
#define _NXTCAM_COLOR_H_

#define NXTCAM_RGB_R 1
#define NXTCAM_RGB_G 2
#define NXTCAM_RGB_B 4

typedef struct {
  uint8_t r,g,b;
} __attribute__ ((packed)) nxtcam_rgb_t;

#endif /* _NXTCAM_COLOR_H_ */
