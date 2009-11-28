/*
    gui/log.c
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
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <gtk/gtk.h>

#include "nxtcam/gui.h"

#define GUI_HEX_LINE_BUFSIZE 256

static gui_gd_t *log_gd = NULL;

void gui_log_set_gd(gui_gd_t *gd) {
  log_gd = gd;
}

void gui_log_add(const char *fmt,...) {
  char *msg;
  va_list args;
  GtkTextIter iter;

  va_start(args,fmt);
  vasprintf(&msg,fmt,args);
  va_end(args);

  if (log_gd!=NULL) {
    gtk_text_buffer_get_end_iter(log_gd->main.log.buf_log,&iter);
    gtk_text_buffer_insert(log_gd->main.log.buf_log,&iter,msg,-1);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(log_gd->main.log.text_log),&iter,0,0.0,0.0,1.0);
  }
  else {
    printf("Log: %s\n",msg);
  }

  free(msg);
}
