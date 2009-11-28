/*
    gui/frame.c
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
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "nxtcam/gui.h"
#include "nxtcam/device.h"
#include "nxtcam/frame.h"

#define GUI_FRAME_MIN_WIDTH  (NXTCAM_FRAME_WIDTH*2)
#define GUI_FRAME_MIN_HEIGHT (NXTCAM_FRAME_HEIGHT*2)

static GdkPixbuf *gui_frame_get_pixbuf(gui_gd_t *gd) {
  const guchar *src;
  int interpolate;
  GdkPixbuf *pixbuf;

  interpolate = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gd->frame.chkbtn_interpolate)) || nxtcam_dev.num_lines<NXTCAM_FRAME_HEIGHT;
  src = (const guchar*)(interpolate?nxtcam_dev.frame:nxtcam_dev.frame_raw);
  pixbuf = gdk_pixbuf_new_from_data(src,GDK_COLORSPACE_RGB,0,8,NXTCAM_FRAME_WIDTH,NXTCAM_FRAME_HEIGHT,NXTCAM_FRAME_WIDTH*3,NULL,NULL);

  return pixbuf;
}

void gui_frame_update(gui_gd_t *gd) {
  GdkPixbuf *pixbuf;
  GdkPixbuf *pixbuf_big;
  int w;
  int h;

  if (GTK_WIDGET_VISIBLE(gd->frame.win)) {
    w = gd->frame.img_frame->allocation.width;
    h = gd->frame.img_frame->allocation.height;

    if (w<GUI_FRAME_MIN_WIDTH || h<GUI_FRAME_MIN_HEIGHT) {
      w = GUI_FRAME_MIN_WIDTH;
      h = GUI_FRAME_MIN_HEIGHT;
      gtk_widget_set_size_request(gd->frame.img_frame,w,h);
    }

    pixbuf = gui_frame_get_pixbuf(gd);
    pixbuf_big = gdk_pixbuf_scale_simple(pixbuf,w,h,GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(GTK_IMAGE(gd->frame.img_frame),pixbuf_big);
    g_object_unref(pixbuf_big);
    g_object_unref(pixbuf);
  }
}

G_MODULE_EXPORT void gui_frame_save(GtkWidget *widget,gui_gd_t *gd) {
  GError *error = NULL;
  GtkWidget *dialog;
  char *filename;
  GtkFileFilter *filter;
  GdkPixbuf *pixbuf;

  if (!nxtcam_com_is_connected()) {
    gui_error(gd,"NXTCam not connected","NXTCam isn't connected");
    return;
  }

  dialog = gtk_file_chooser_dialog_new("Save as...",GTK_WINDOW(gd->frame.win),GTK_FILE_CHOOSER_ACTION_SAVE,
                                       "gtk-cancel",GTK_RESPONSE_CANCEL,
                                       "gtk-save",GTK_RESPONSE_ACCEPT,
                                       NULL);
  gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog),0);
  gui_file_filter_add_writable_pixbufs(GTK_FILE_CHOOSER(dialog));

  if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(dialog));

    pixbuf = gui_frame_get_pixbuf(gd);

    if (!gdk_pixbuf_save(pixbuf,filename,gtk_file_filter_get_name(filter),&error,NULL)) {
      gui_gerror(gd,error);
    }

    free(filename);
  }

  gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void gui_frame_capture(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_dev_req_frame();
}

G_MODULE_EXPORT void gui_frame_pickcolor(GtkWidget *widget,gui_gd_t *gd) {

}

G_MODULE_EXPORT void gui_frame_close(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_hide(gd->frame.win);
}
