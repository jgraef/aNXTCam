/*
    com.h
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

#ifndef _NXTCAM_COM_H_
#define _NXTCAM_COM_H_

#include <stddef.h>
#include <stdarg.h>

#define NXTCAM_COM_BUFSIZE 256

int nxtcam_com_open(const char *device);
void nxtcam_com_close(void);
int nxtcam_com_is_connected(void);
const char *nxtcam_com_read(size_t *size);
int nxtcam_com_write(const char *line);
int nxtcam_com_vwritef(const char *fmt,va_list args);
int nxtcam_com_writef(const char *fmt,...);

#endif /* _NXTCAM_COM_H_ */
