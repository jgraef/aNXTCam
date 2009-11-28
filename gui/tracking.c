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
#include <gdk/gdk.h>
#include <glib.h>

#include "nxtcam/gui.h"
#include "nxtcam/device.h"
#include "nxtcam/object.h"

#define GUI_TRACKING_MIN_WIDTH  (NXTCAM_FRAME_WIDTH*2)
#define GUI_TRACKING_MIN_HEIGHT (NXTCAM_FRAME_HEIGHT*2)

G_MODULE_EXPORT gboolean gui_tracking_delete_event(GtkWidget *widget,GdkEvent *event,gui_gd_t *gd) {
  nxtcam_dev_req_tracking(0);
  gtk_widget_hide(widget);
}

G_MODULE_EXPORT void gui_tracking_start(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_dev_req_tracking(1);
}

G_MODULE_EXPORT void gui_tracking_stop(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_dev_req_tracking(0);
}

G_MODULE_EXPORT void gui_tracking_record(GtkWidget *widget,gui_gd_t *gd) {

}

G_MODULE_EXPORT void gui_tracking_mode(GtkWidget *widget,gui_gd_t *gd) {
  int mode;

  // get mode
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gd->tracking.radio_mode_obj))) {
    mode = NXTCAM_TRACK_OBJECT;
  }
  else {
    mode = NXTCAM_TRACK_LINE;
  }

  // set mode
  nxtcam_dev_req_tracking_type(mode);
}

G_MODULE_EXPORT gboolean gui_tracking_expose(GtkWidget *widget,GdkEventExpose *event,gui_gd_t *gd) {
  GdkColor color_obj;
  int i,w,h,threshold,filter_oversized;
  double sx,sy,s,rl,rh,gl,gh,bl,bh;

  // prepare filtering
  threshold = gtk_range_get_value(GTK_RANGE(gd->tracking.scale_threshold));
  filter_oversized = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gd->tracking.chkbtn_filter));

  // prepare sizing
  w = gd->tracking.draw_objects->allocation.width;
  h = gd->tracking.draw_objects->allocation.height;

  if (w<GUI_TRACKING_MIN_WIDTH || h<GUI_TRACKING_MIN_HEIGHT) {
    w = GUI_TRACKING_MIN_WIDTH;
    h = GUI_TRACKING_MIN_HEIGHT;
    gtk_widget_set_size_request(gd->tracking.draw_objects,w,h);
  }

  sx = ((double)w)/((double)NXTCAM_FRAME_WIDTH);
  sy = ((double)h)/((double)NXTCAM_FRAME_HEIGHT);
  s = sx<sy?sx:sy;

  // Background
  gdk_draw_rectangle(gd->tracking.draw_objects->window,
                     widget->style->bg_gc[GTK_WIDGET_STATE(gd->tracking.draw_objects)],
                     TRUE,0,0,w,h);

  // Draw objects
  for (i=0;i<nxtcam_dev.num_objects;i++) {
    if (!(filter_oversized && NXTCAM_OBJECT_IS_OVERSIZED(nxtcam_dev.objects+i))
     && (nxtcam_dev.objects[i].w>threshold && nxtcam_dev.objects[i].h>threshold)) {

      color_range_widget_get_color_low(gd->colormap.colorrange,nxtcam_dev.objects[i].color,&rl,&gl,&bl);
      color_range_widget_get_color_high(gd->colormap.colorrange,nxtcam_dev.objects[i].color,&rh,&gh,&bh);
      color_obj.red = (int)((rl+rh)*32767.5);
      color_obj.green = (int)((gl+gh)*32767.5);
      color_obj.blue = (int)((bl+bh)*32767.5);

      gdk_gc_set_rgb_fg_color(gd->tracking.gc_obj,&color_obj);
      gdk_draw_rectangle(gd->tracking.draw_objects->window,gd->tracking.gc_obj,TRUE,
                         s*nxtcam_dev.objects[i].x,s*nxtcam_dev.objects[i].y,
                         s*nxtcam_dev.objects[i].w,s*nxtcam_dev.objects[i].h);

PangoLayout *layout;
char text[64];

      snprintf(text,64,"(%d, %d)\n(%d, %d)",nxtcam_dev.objects[i].x,
                                    nxtcam_dev.objects[i].y,
                                    nxtcam_dev.objects[i].w,
                                    nxtcam_dev.objects[i].h);
      layout = gtk_widget_create_pango_layout(widget,text);
      gdk_draw_layout(gd->tracking.draw_objects->window,
                      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                      s*nxtcam_dev.objects[i].x,s*nxtcam_dev.objects[i].y,
                      layout);
    }
  }
}

G_MODULE_EXPORT void gui_tracking_realize(GtkWidget *widget,gui_gd_t *gd) {
  gd->tracking.gc_obj = gdk_gc_new(gd->tracking.draw_objects->window);
}

void gui_tracking_update(gui_gd_t *gd) {
  if (GTK_WIDGET_VISIBLE(gd->tracking.win)) {
    gui_tracking_expose(gd->tracking.draw_objects,NULL,gd);
  }
}
