/*
    colormap.c
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

#include "nxtcam/colormap.h"

int nxtcam_colormap_save(const char *file,nxtcam_colormap_t *colormap) {
  FILE *fd;

  fd = fopen(file,"w");
  if (fd==NULL) {
    return -1;
  }

  fputs(NXTCAM_COLORMAP_FILESIG,fd);
  fwrite(colormap,1,sizeof(nxtcam_colormap_t),fd);
  fclose(fd);
  return 0;
}

int nxtcam_colormap_open(const char *file,nxtcam_colormap_t *colormap) {
  FILE *fd;
  char sig[32];

  fd = fopen(file,"r");
  if (fd==NULL) {
    printf("datei oeffnen\n");
    return -1;
  }

  fgets(sig,32,fd);
  if (strcmp(NXTCAM_COLORMAP_FILESIG,sig)!=0) {
    printf("Signatur\n");
    fclose(fd);
    return -1;
  }

  if (fread(colormap,sizeof(nxtcam_colormap_t),1,fd)!=1) {
    printf("groesse colormap\n");
    fclose(fd);
    return -1;
  }

  fclose(fd);
  return 0;
}


int nxtcam_colormap_range(nxtcam_colormap_t *colormap,int r1,int r2,int g1,int g2,int b1,int b2) {

}
