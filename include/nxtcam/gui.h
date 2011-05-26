/*
    gui.h
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

#ifndef _NXTCAM_GUI_H_
#define _NXTCAM_GUI_H_

#include <nxtcam/colormap.h>
#include <nxtcam/object.h>

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

struct gui_preferences {
  int auto_white_balance;
  int auto_adjust_mode;
  int flourescent_light_filter;
  char *device;
};

typedef struct {
  int sync;

  struct {
    GtkWidget *win;
    struct {
      GtkTextBuffer *buf_log;
      GtkWidget *text_log;
    } log;
    struct {
      GtkWidget *statusbar;
      int contextid;
      GtkWidget *img_led;
    } statusbar;
  } main;

  struct {
    GtkWidget *win;
    GtkWidget *tolerance; //widget for the spin button for tolerance
    GtkWidget *alertOverlap; //label to alert overlaping colors
    GtkWidget *box_colorrange;
    GtkWidget *box_colormap;
    GtkWidget *colorrange;
    char *current;
  } colormap;

  struct {
    GtkWidget *win;
    GtkWidget *img_frame;
    GtkWidget *chkbtn_interpolate;
    GtkWidget *aspect_frame;
  } frame;

  struct {
    GtkWidget *win;
    GtkWidget *draw_objects;
    GtkWidget *radio_mode_obj;
    GtkWidget *radio_mode_line;
    GtkWidget *scale_threshold;
    GtkWidget *chkbtn_filter;
    GdkGC *gc_obj;
  } tracking;

  struct {
    GtkWidget *win;
    GtkWidget *chkbtn_whitebalance;
    GtkWidget *chkbtn_adjustmode;
    GtkWidget *chkbtn_lightfilter;
    GtkWidget *btn_device;
    struct gui_preferences current;
  } preferences;

  struct {
    GtkWidget *win;
    GtkWidget *text;
  } version;

  struct {
    GtkWidget *win;
    GtkWidget *text;
  } ping;

  struct {
    GtkWidget *dialog;
  } about;

} gui_gd_t;

// Main
gui_gd_t *gui_init(int *argc,char ***argv);
void gui_main(gui_gd_t *gd);
void gui_file_filter_add_writable_pixbufs(GtkFileChooser *filechooser);
const char *gui_file_filter_get_pixbuf_format_name(GtkFileFilter *filter);
void gui_file_filter_add_objtrack(GtkFileChooser *filechooser);
void gui_file_filter_add_colormap(GtkFileChooser *filechooser);

// Colormap
void gui_colormap_autosave(gui_gd_t *gd);
void gui_colormap_autoload(gui_gd_t *gd);
void gui_colormap_upload(GtkWidget *widget,gui_gd_t *gd);

// Log
void gui_log_set_gd(gui_gd_t *gd);
void gui_log_add(const char *fmt,...);
void gui_hex_add(void *buf,size_t n,int sending);

// Message box
#define gui_question(gd,title,...) gui_msgbox(gd,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,title,__VA_ARGS__)
#define gui_info(gd,title,...)     gui_msgbox(gd,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,title,__VA_ARGS__)
#define gui_error(gd,title,...)    gui_msgbox(gd,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,title,__VA_ARGS__)
#define gui_warning(gd,title,...)  gui_msgbox(gd,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,title,__VA_ARGS__)
#define gui_gerror(gd,e)           gui_error(gd,g_quark_to_string((e)->domain),"Error #%d:\n%s",(e)->code,(e)->message)
int gui_msgbox(gui_gd_t *gd,GtkMessageType type,GtkButtonsType buttons,const char *title,const char *fmt,...);

// Frame
void gui_frame_update(gui_gd_t *gd);
void gui_frame_capture(GtkWidget *widget,gui_gd_t *gd);

// Tracking
void gui_tracking_update(gui_gd_t *gd);

// Version
void gui_version_update(gui_gd_t *gd);
void gui_version_query(GtkWidget *widget,gui_gd_t *gd);

// Ping
void gui_ping_update(gui_gd_t *gd);
void gui_ping_ping(GtkWidget *widget,gui_gd_t *gd);

// Preferences
int gui_preferences_open(const char *filename,struct gui_preferences *preferences);
void gui_preferences_init(gui_gd_t *gd);

// Statusbar
void gui_statusbar_msg(gui_gd_t *gd,const char *fmt,...);
void gui_statusbar_led(gui_gd_t *gd,int connected);

#endif /* _NXTCAM_GUI_H_ */
