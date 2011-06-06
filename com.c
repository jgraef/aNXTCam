/*
    com.c
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

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

#include "nxtcam/com.h"
#include "nxtcam/log.h"

static struct {
  int fd;
  const char *dev;
  struct termios tio;
  struct termios oldtio;
} nxtcam_com = {
  .fd = -1
};

int nxtcam_com_open(const char *device) {
  int fd;

  if (nxtcam_com.fd==-1) {
    // open COM device
    fd = open(device,O_RDWR|O_NOCTTY);
    if (fd==-1) {
      return -1;
    }

    // configure COM device
    tcgetattr(fd,&nxtcam_com.oldtio);
	
    memset(&nxtcam_com.tio,0,sizeof(nxtcam_com.tio));
    nxtcam_com.tio.c_cflag = CLOCAL|CREAD;
    nxtcam_com.tio.c_iflag = IGNPAR;
    nxtcam_com.tio.c_oflag = 0;
    nxtcam_com.tio.c_lflag = 0;//ICANON;
    cfsetospeed(&nxtcam_com.tio,B115200);            // 115200 baud
    cfsetispeed(&nxtcam_com.tio,B115200);            // 115200 baud
    tcflush(fd,TCIFLUSH);
    tcsetattr(fd,TCSANOW,&nxtcam_com.tio);

    // put into nxtcam_com structure
    nxtcam_com.dev = device;
    nxtcam_com.fd = fd;

    logmsg("Connected\n");
  }

  return 0;
}

void nxtcam_com_close(void) {
  int fd;

  if (nxtcam_com.fd!=-1) {
    fd = nxtcam_com.fd;

    nxtcam_com.fd = -1;

    tcsetattr(fd,TCSANOW,&nxtcam_com.oldtio);
    close(fd);

    logmsg("Disconnected\n");
  }
}

int nxtcam_com_is_connected(void) {
  return nxtcam_com.fd!=-1;
}

const char *nxtcam_com_read(size_t *size) {
  static char buf[NXTCAM_COM_BUFSIZE];
  int n = 0;
  char b,endbyte;

  if (nxtcam_com.fd==-1) {
    return NULL;
  }

  do {
    while (read(nxtcam_com.fd,&b,1)==0) {
      usleep(10000);
    }

    if (n==0) {
      if (b==0x0A) {
        endbyte = 0xFF; // tracking data
      }
      else if (b==0x0B) {
        endbyte = 0x0F; // Frame data
      }
      else { // ASCII data (default)
        endbyte = '\r';
      }
    }

    buf[n++] = b;
  } while ((b!=endbyte && n<NXTCAM_COM_BUFSIZE) || (n==2 && endbyte==0x0F && b==endbyte));
  // last case: (n==2 && endbyte==0x0F && b==endbyte)
  // normally it would abort here, but the 0x0F doens't could
  // as end byte this time, since it's meant as line number

  buf[n-1] = 0;

  if (size!=NULL) {
    *size = n;
  }

  return buf;
}

int nxtcam_com_write(const char *line) {
  int n;

  if (nxtcam_com.fd==-1) {
    return -1;
  }

  n = strlen(line);
  n = write(nxtcam_com.fd,line,n);
  write(nxtcam_com.fd,"\r\n",n);

  return n;
}

int nxtcam_com_vwritef(const char *fmt,va_list args) {
  char buf[NXTCAM_COM_BUFSIZE];

  vsnprintf(buf,NXTCAM_COM_BUFSIZE,fmt,args);
  return nxtcam_com_write(buf);
}

int nxtcam_com_writef(const char *fmt,...) {
  va_list args;
  int ret;

  va_start(args,fmt);
  ret = nxtcam_com_vwritef(fmt,args);
  va_end(args);

  return ret;
}
