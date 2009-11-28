/*
    gui/statusbar.c
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
#include <stdlib.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "nxtcam/gui.h"

void gui_statusbar_msg(gui_gd_t *gd,const char *fmt,...) {
  char *msg;
  va_list args;

  va_start(args,fmt);
  vasprintf(&msg,fmt,args);

  gtk_statusbar_pop(GTK_STATUSBAR(gd->main.statusbar.statusbar),gd->main.statusbar.contextid);
  gtk_statusbar_push(GTK_STATUSBAR(gd->main.statusbar.statusbar),gd->main.statusbar.contextid,msg);

  free(msg);
  va_end(args);
}

void gui_statusbar_led(gui_gd_t *gd,int connected) {
  gtk_image_set_from_file(GTK_IMAGE(gd->main.statusbar.img_led),connected?"led_green.png":"led_red.png");
}
