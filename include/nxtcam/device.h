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

#ifndef _NXTCAM_DEVICE_H_
#define _NXTCAM_DEVICE_H_

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/time.h>

#include "nxtcam/colormap.h"
#include "nxtcam/object.h"
#include "nxtcam/frame.h"

#define NXTCAM_DEV_BUFSIZE 64

#define NXTCAM_DEV_DATATYPE_TRACK 0x0A
#define NXTCAM_DEV_DATATYPE_FRAME 0x0B

#define NXTCAM_MODE_IDLE      0
#define NXTCAM_MODE_FRAMEDUMP 1
#define NXTCAM_MODE_TRACKING  2

#define NXTCAM_NONE 0
#define NXTCAM_ACK  1
#define NXTCAM_NCK  2

#define NXTCAM_TRACK_LINE   1
#define NXTCAM_TRACK_OBJECT 2

#define NXTCAM_PING_PINGING 1
#define NXTCAM_PING_SUCCESS 2

// Allows writing to NXTCam registers
#define NXTCAM_ALLOW_REGW

struct {
  /// Whether reading thread should run
  int read;

  /// Reader thread
  pthread_t thread;

  /// Current mode
  int mode;

  /// Last response status
  int ack;

  /// Version that was responded to last request
  char version[32];

  /// Ping times
  int ping_status;
  struct timeval ping_start;
  struct timeval ping_end;

  /// Frame that was received by last frame dump
  size_t num_lines;
  nxtcam_frame_t frame;
  nxtcam_frame_t frame_raw;

  /// Tracking data of objects
  size_t num_objects;
  nxtcam_object_t objects[8];
} nxtcam_dev;


int nxtcam_dev_init(void);
int nxtcam_dev_flush(void);
int nxtcam_dev_abort(void);
int nxtcam_dev_req_ping(void);
int nxtcam_dev_req_regw(int reg,int val);
int nxtcam_dev_req_frame(void);
int nxtcam_dev_req_version(void);
int nxtcam_dev_req_tracking(int enable);
int nxtcam_dev_req_tracking_type(int type);
int nxtcam_dev_req_reset(void);
int nxtcam_dev_req_colormap(nxtcam_colormap_t *cm);


static __inline__ void nxtcam_mode_add(int add) {
  nxtcam_dev.mode |= add;
}

static __inline__ void nxtcam_mode_remove(int remove) {
  nxtcam_dev.mode &= ~remove;
}

static __inline__ int nxtcam_mode_is(int is) {
  return nxtcam_dev.mode&is;
}

#endif /* _NXTCAM_DEVICE_H_ */
