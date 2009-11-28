/*
    os.h
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

#ifndef _NXTCAM_OS_H_
#define _NXTCAM_OS_H_

/* if you want aNXTCam for another OS,   */
/* send me a mail: janosch.graef@gmx.net */

/* To define which OS you are using, set */
/* e.g. -D__LINUX__ as compiler flag     */

#if defined(__WINDOWS__)
  #define NXTCAM_OS_NAME           "Windows"
  #define NXTCAM_OS_DEFAULT_DEVICE "COM0"

#else /* Default: Linux (Flag: "-D__LINUX__") */
  #define NXTCAM_OS_NAME           "Linux"
  #define NXTCAM_OS_DEFAULT_DEVICE "/dev/ttyUSB0"

#endif

#endif /* _NXTCAM_OS_H_ */
