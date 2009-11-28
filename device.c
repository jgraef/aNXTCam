/*
    device.c
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
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

#include "nxtcam/device.h"
#include "nxtcam/com.h"
#include "nxtcam/log.h"
#include "nxtcam/frame.h"
#include "nxtcam/color.h"
#include "nxtcam/object.h"


static void nxtcam_dev_resp_frame(const char *data,size_t n) {
  nxtcam_rgb_t *p1;
  nxtcam_rgb_t *p2;
  int line,x;

  line = data[1]*2;
  //logmsg("Receiving line %d and %d of %d\n",line,line+1,NXTCAM_FRAME_HEIGHT);

  for (x=0;x<NXTCAM_FRAME_WIDTH;x++) {
    p1 = nxtcam_frame_pixel(nxtcam_dev.frame,x,line);
    p2 = nxtcam_frame_pixel(nxtcam_dev.frame,x,line+1);

    if (x%2==0) {
      p1->g = data[x+2]&0xF0;
      p2->b = (data[x+2]<<4)&0xF0;
    }
    else {
      p1->r = data[x+2]&0xF0;
      p2->g = (data[x+2]<<4)&0xF0;
    }
  }

  nxtcam_dev.num_lines += 2;

  if (nxtcam_dev.num_lines==NXTCAM_FRAME_HEIGHT) {
    nxtcam_frame_copy(nxtcam_dev.frame_raw,nxtcam_dev.frame);
    nxtcam_frame_bayer_interpolate(nxtcam_dev.frame);
    nxtcam_mode_remove(NXTCAM_MODE_FRAMEDUMP);
  }
}

static void nxtcam_dev_resp_tracking(const char *data,size_t n) {
  int i,num_objects;
  const unsigned char *objdata;

  num_objects = data[1];

  for (i=0;i<num_objects;i++) {
    objdata = data+2+i*5;

    nxtcam_dev.objects[i].id = i;
    nxtcam_dev.objects[i].color = objdata[0];
    nxtcam_dev.objects[i].x = objdata[1];
    nxtcam_dev.objects[i].y = objdata[2];
    nxtcam_dev.objects[i].x2 = objdata[3];
    nxtcam_dev.objects[i].y2 = objdata[4];
    nxtcam_dev.objects[i].w = nxtcam_dev.objects[i].x2-nxtcam_dev.objects[i].x+1;
    nxtcam_dev.objects[i].h = nxtcam_dev.objects[i].y2-nxtcam_dev.objects[i].y+1;
  }

  // NOTE updated as last because otherwise another thread could
  //      read when the new number of objects is updated and is
  //      bigger then the old one and the object data isn't update
  //      yet. Then the thread would read invalid object data
  nxtcam_dev.num_objects = num_objects;
}

static void *nxtcam_dev_reader(void *unused) {
  const char *data;
  size_t n;

  nxtcam_dev.read = 1;
  while (nxtcam_dev.read) {
    // read data
    data = nxtcam_com_read(&n);

    if (data!=NULL && *data!=0) {
      if (data[0]==0x0A) {
        if (nxtcam_mode_is(NXTCAM_MODE_TRACKING)) {
          nxtcam_dev_resp_tracking(data,n);
        }
      }
      else if (data[0]==0x0B) {
        if (nxtcam_mode_is(NXTCAM_MODE_FRAMEDUMP)) {
          nxtcam_dev_resp_frame(data,n);
        }
      }
      else if (isprint(data[0])) {
        if (memcmp(data,"ACK",3)==0) {
          logmsg("Received ACK\n");
          nxtcam_dev.ack = NXTCAM_ACK;

          // ping successful (when pinging)
          if (nxtcam_dev.ping_status==NXTCAM_PING_PINGING) {
            gettimeofday(&nxtcam_dev.ping_end,NULL);
            nxtcam_dev.ping_status = NXTCAM_PING_SUCCESS;
          }
        }
        else if (memcmp(data,"NCK",3)==0) {
          logmsg("Received NCK\n");
          nxtcam_dev.ack = NXTCAM_NCK;
        }
        else if (memcmp(data,"NXT",3)==0 || memcmp(data,"AVR",3)==0) {
          strncpy(nxtcam_dev.version,data,sizeof(nxtcam_dev.version));
        }
        else {
          logmsg("Unknown message. First byte: 0x%02X\n",data[0]&0xFF);
        }
      }
      else {
        logmsg("Invalid data type: 0x%02X\n",data[0]&0xFF);
      }
    }
    else {
      usleep(10000);
    }

    pthread_testcancel();
  }

  return NULL;
}

static void nxtcam_dev_shutdown(void) {
  pthread_cancel(nxtcam_dev.thread);
  nxtcam_frame_free(nxtcam_dev.frame);
}

int nxtcam_dev_init(void) {
  memset(&nxtcam_dev,0,sizeof(nxtcam_dev));

  nxtcam_dev.frame = nxtcam_frame_alloc();
  nxtcam_dev.frame_raw = nxtcam_frame_alloc();

  pthread_create(&nxtcam_dev.thread,NULL,nxtcam_dev_reader,NULL);

  return 0;
}

int nxtcam_dev_flush(void) {
  if (nxtcam_com_is_connected()) {
    return nxtcam_com_write("");
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_abort(void) {
  if (nxtcam_com_is_connected()) {
    return nxtcam_com_write("NCK");
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_req_ping(void) {
  if (nxtcam_com_is_connected()) {
    if (nxtcam_dev.ping_status!=NXTCAM_PING_PINGING) {
      logmsg("Pinging\n");
      gettimeofday(&nxtcam_dev.ping_start,NULL);
      nxtcam_dev.ack = 0;
      nxtcam_dev.ping_status = NXTCAM_PING_PINGING;
      return nxtcam_com_write("PG");
    }
    else {
    logmsg("Already pinging\n");
    return -1;
    }
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_req_regw(int reg,int val) {
#ifdef NXTCAM_ALLOW_REGW
  if (nxtcam_com_is_connected()) {
    val = val&0xFF;
    logmsg("Changing register 0x%X to 0x%02X\n",reg,val);
    return nxtcam_com_writef("CR %d %d",reg&0xFF,val)>0?0:-1;
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
#else
  logmsg("Changing registers is disabled (at compile-time)\n");
  return -1;
#endif
}

int nxtcam_dev_req_frame(void) {
  if (nxtcam_com_is_connected()) {
    logmsg("Dumping frame\n");
    nxtcam_dev.num_lines = 0;
    nxtcam_frame_clear(nxtcam_dev.frame);
    nxtcam_frame_clear(nxtcam_dev.frame_raw);
    nxtcam_mode_add(NXTCAM_MODE_FRAMEDUMP);
    return nxtcam_com_write("DF")>0?0:-1;
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_req_version(void) {
  if (nxtcam_com_is_connected()) {
    logmsg("Get version\n");
    nxtcam_dev.version[0] = 0;
    return nxtcam_com_write("GV");
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_req_tracking(int enable) {
  if (nxtcam_com_is_connected()) {
    if (enable) {
      nxtcam_mode_add(NXTCAM_MODE_TRACKING);
      logmsg("Enable tracking\n");
      return nxtcam_com_write("ET");
    }
    else {
      nxtcam_mode_remove(NXTCAM_MODE_TRACKING);
      nxtcam_dev.num_objects = 0;
      logmsg("Disable tracking\n");
      return nxtcam_com_write("DT");
    }
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_req_tracking_type(int type) {
  if (nxtcam_com_is_connected()) {
    logmsg("Selected %s tracking\n",type==NXTCAM_TRACK_LINE?"line":"object");
    nxtcam_com_write(type==NXTCAM_TRACK_LINE?"TL":"TO");
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_req_reset(void) {
  if (nxtcam_com_is_connected()) {
    logmsg("Resetting NXTCam\n");
    nxtcam_com_write("RS");
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}

int nxtcam_dev_req_colormap(nxtcam_colormap_t *cm) {
  if (nxtcam_com_is_connected()) {
    if (!nxtcam_mode_is(NXTCAM_MODE_TRACKING)) {
      logmsg("Uploading colormap\n");
      return nxtcam_com_writef("SM "
      /* Format * R */ "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
               /* G */ "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
               /* B */ "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      /* Values * R */ cm->r[0],cm->r[1],cm->r[2],cm->r[3],cm->r[4],cm->r[5],cm->r[6],cm->r[7],cm->r[8],cm->r[9],cm->r[10],cm->r[11],cm->r[12],cm->r[13],cm->r[14],cm->r[15],
               /* G */ cm->g[0],cm->g[1],cm->g[2],cm->g[3],cm->g[4],cm->g[5],cm->g[6],cm->g[7],cm->g[8],cm->g[9],cm->g[10],cm->g[11],cm->g[12],cm->g[13],cm->g[14],cm->g[15],
               /* B */ cm->b[0],cm->b[1],cm->b[2],cm->b[3],cm->b[4],cm->b[5],cm->b[6],cm->b[7],cm->b[8],cm->b[9],cm->b[10],cm->b[11],cm->b[12],cm->b[13],cm->b[14],cm->b[15]);
    }
    else {
      logmsg("Cannot upload colormap while tracking is enabled\n");
      return -1;
    }
  }
  else {
    logmsg("NXTCam not connected\n");
    return -1;
  }
}
