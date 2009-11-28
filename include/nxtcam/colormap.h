/*
    colormap.h
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

#ifndef _NXTCAM_COLORMAP_H_
#define _NXTCAM_COLORMAP_H_

#include <stdint.h>

#define NXTCAM_COLORMAP_FILESIG "#aNXTCam COLORMAP\n"

typedef struct {
  uint8_t r[16];
  uint8_t g[16];
  uint8_t b[16];
} nxtcam_colormap_t;

int nxtcam_colormap_save(const char *file,nxtcam_colormap_t *colormap);
int nxtcam_colormap_open(const char *file,nxtcam_colormap_t *colormap);

#endif /* _NXTCAM_COLORMAP_H_ */
