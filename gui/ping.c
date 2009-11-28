/*
    gui/ping.c
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
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "nxtcam/gui.h"
#include "nxtcam/device.h"

void gui_ping_update(gui_gd_t *gd) {
  char *text;
  double ping_time;

  if (GTK_WIDGET_VISIBLE(gd->ping.win)) {
    if (!nxtcam_com_is_connected()) {
      gtk_entry_set_text(GTK_ENTRY(gd->ping.text),"NXTCam not connected");
    }
    else if (nxtcam_dev.ping_status==NXTCAM_PING_PINGING) {
      text = strdup("Pinging...");
      gtk_entry_set_text(GTK_ENTRY(gd->ping.text),"Pinging...");
    }
    else if (nxtcam_dev.ping_status==NXTCAM_PING_SUCCESS) {
      ping_time = (((double)nxtcam_dev.ping_end.tv_sec)*1000.+((double)nxtcam_dev.ping_end.tv_usec)/1000.)
                 -(((double)nxtcam_dev.ping_start.tv_sec)*1000.+((double)nxtcam_dev.ping_start.tv_usec)/1000.);

      asprintf(&text,"Success: %.3f ms",ping_time);
      gtk_entry_set_text(GTK_ENTRY(gd->ping.text),text);
      free(text);
    }
    else {
      gtk_entry_set_text(GTK_ENTRY(gd->ping.text),"");
    }
  }
}

G_MODULE_EXPORT void gui_ping_ping(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_dev_req_ping();
}

G_MODULE_EXPORT void gui_ping_ok(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_hide(gd->ping.win);
}
