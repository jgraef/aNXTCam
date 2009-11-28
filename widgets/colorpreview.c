/*
    colorpreview.c
    aNXTCam - a NXTCam configuration tool
    Color Range widget for Gtk+
    Copyright (C) 2009  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <math.h>

#include <gdk/gdk.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkdrawingarea.h>

#include "colorpreview/colorpreview.h"

static gboolean color_preview_signal_expose_event(GtkWidget *widget,GdkEventExpose *event,ColorPreview *clpreview) {
  cairo_t *cr;
  cairo_pattern_t *pat;
  gdouble w,h;

  w = (gdouble)widget->allocation.width;
  h = (gdouble)widget->allocation.height;

  /* get cairo context */
  cr = gdk_cairo_create(widget->window);

  /* set a clip region for the expose event */
  cairo_rectangle(cr,event->area.x,event->area.y,event->area.width,event->area.height);
  cairo_clip(cr);

  /* clear background */
  pat = cairo_pattern_create_linear(0.,0.,w,0.);
  cairo_pattern_add_color_stop_rgb(pat,0.,clpreview->color_low.r,
                                          clpreview->color_low.g,
                                          clpreview->color_low.b);
  cairo_pattern_add_color_stop_rgb(pat,1.,clpreview->color_high.r,
                                          clpreview->color_high.g,
                                          clpreview->color_high.b);
  cairo_set_source(cr,pat);
  cairo_rectangle(cr,0.,0.,w,h);
  cairo_fill(cr);
  cairo_pattern_destroy(pat);

  return TRUE;
}

static void color_preview_expose(ColorPreview *clpreview) {
  GdkRectangle rect;
  GtkWidget *widget;

  widget = GTK_WIDGET(clpreview);

  if (GDK_IS_WINDOW(widget->window)) {
    rect.x = 0;
    rect.y = 0;
    rect.width = widget->allocation.width;
    rect.height = widget->allocation.height;

    gdk_window_invalidate_rect(widget->window,&rect,FALSE);
  }
}

static void color_preview_signal_realize(GtkWidget *widget,ColorPreview *clpreview) {
  /* set size request */
  gtk_widget_set_size_request(widget,clpreview->min_w,clpreview->min_h);
}

static void color_preview_destroy(GtkObject *object) {
  ColorPreview *clpreview;
  ColorPreviewClass *klass;

  g_return_if_fail(object!=NULL);
  g_return_if_fail(IS_COLOR_PREVIEW(object));
  clpreview = COLOR_PREVIEW(object);

  klass = gtk_type_class(gtk_widget_get_type());

  if (GTK_OBJECT_CLASS(klass)->destroy) {
     (* GTK_OBJECT_CLASS(klass)->destroy) (object);
  }

}

static void color_preview_class_init(ColorPreviewClass *klass) {
  GtkObjectClass *object_class;

  object_class = GTK_OBJECT_CLASS(klass);
  object_class->destroy = color_preview_destroy;
}

static void color_preview_init(ColorPreview *clpreview) {
  /* set defaults */
  clpreview->min_w = 20;
  clpreview->min_h = 20;
  clpreview->color_low.r = 0.;
  clpreview->color_low.g = 0.;
  clpreview->color_low.b = 0.;
  clpreview->color_high.r = 0.;
  clpreview->color_high.g = 0.;
  clpreview->color_high.b = 0.;

  /* add events */
  gtk_widget_add_events(GTK_WIDGET(clpreview),GDK_POINTER_MOTION_MASK
                                            | GDK_POINTER_MOTION_HINT_MASK
                                            | GDK_BUTTON_PRESS_MASK
                                            | GDK_BUTTON_RELEASE_MASK
                                            | GDK_ENTER_NOTIFY_MASK
                                            | GDK_LEAVE_NOTIFY_MASK
                                            | GDK_FOCUS_CHANGE_MASK);

  /* connect signals handlers */
  g_signal_connect(G_OBJECT(clpreview),"realize",G_CALLBACK(color_preview_signal_realize),clpreview);
  g_signal_connect(G_OBJECT(clpreview),"expose-event",G_CALLBACK(color_preview_signal_expose_event),clpreview);
}

GType color_preview_get_type() {
  static GType clpreview_type = 0;

  if (clpreview_type==0) {
    GTypeInfo clpreview_info = {
      sizeof(ColorPreviewClass),
      NULL,
      NULL,
      (GClassInitFunc)color_preview_class_init,
      NULL,
      NULL,
      sizeof(ColorPreview),
      0,
      (GInstanceInitFunc)color_preview_init
    };

    clpreview_type = g_type_register_static(GTK_TYPE_DRAWING_AREA,"ColorPreview",&clpreview_info,0);
  }

  return clpreview_type;
}


GtkWidget *color_preview_new() {
  ColorPreview *clpreview;

  clpreview = COLOR_PREVIEW(g_object_new(COLOR_PREVIEW_TYPE,NULL));

  return GTK_WIDGET(clpreview);
}

void color_preview_get_color_low(ColorPreview *clpreview,gdouble *r,gdouble *g,gdouble *b) {
  g_return_if_fail(clpreview!=NULL);
  g_return_if_fail(IS_COLOR_PREVIEW(clpreview));

  if (r!=NULL) {
    *r = clpreview->color_low.r;
  }
  if (g!=NULL) {
    *g = clpreview->color_low.g;
  }
  if (b!=NULL) {
    *b = clpreview->color_low.b;
  }
}

void color_preview_set_color_low(ColorPreview *clpreview,gdouble r,gdouble g,gdouble b) {
  g_return_if_fail(clpreview!=NULL);
  g_return_if_fail(IS_COLOR_PREVIEW(clpreview));

  clpreview->color_low.r = r;
  clpreview->color_low.g = g;
  clpreview->color_low.b = b;

  color_preview_expose(clpreview);
}

void color_preview_get_color_high(ColorPreview *clpreview,gdouble *r,gdouble *g,gdouble *b) {
  g_return_if_fail(clpreview!=NULL);
  g_return_if_fail(IS_COLOR_PREVIEW(clpreview));

  if (r!=NULL) {
    *r = clpreview->color_high.r;
  }
  if (g!=NULL) {
    *g = clpreview->color_high.g;
  }
  if (b!=NULL) {
    *b = clpreview->color_high.b;
  }
}

void color_preview_set_color_high(ColorPreview *clpreview,gdouble r,gdouble g,gdouble b) {
  g_return_if_fail(clpreview!=NULL);
  g_return_if_fail(IS_COLOR_PREVIEW(clpreview));

  clpreview->color_high.r = r;
  clpreview->color_high.g = g;
  clpreview->color_high.b = b;

  color_preview_expose(clpreview);
}
