/*
    gui/tracking.c
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

#include <gtk/gtk.h>
#include <glib.h>

#include "nxtcam/gui.h"
#include "nxtcam/device.h"

void gui_version_update(gui_gd_t *gd) {
  const char *text;

  if (GTK_WIDGET_VISIBLE(gd->version.win)) {
    if (!nxtcam_com_is_connected()) {
      text = "NXTCam not connected";
    }
    else if (nxtcam_dev.version[0]==0) {
      text = "";
    }
    else {
      text = nxtcam_dev.version;
    }

    gtk_entry_set_text(GTK_ENTRY(gd->version.text),text);
  }
}

G_MODULE_EXPORT void gui_version_query(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_dev_req_version();
}

G_MODULE_EXPORT void gui_version_ok(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_hide(gd->version.win);
}
