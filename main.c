/*
    main.c
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
#include <unistd.h>

#include "nxtcam/device.h"
#include "nxtcam/gui.h"

int main(int argc,char *argv[]) {
  gui_gd_t *gd;

  // TODO parse command line options

  // init GUI
  gd = gui_init(&argc,&argv);
  if (gd==NULL) {
    return 1;
  }

  // init NXTCam
  if (nxtcam_dev_init()==-1) {
    fprintf(stderr,"Could not initialize device driver\n");
    return 1;
  }

  // run GUI
  gui_main(gd);

  return 0;
}
