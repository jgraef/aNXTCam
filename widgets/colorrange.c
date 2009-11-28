/*
    colorrange.c
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

#include "colorrange/colorrange.h"

#include "colorrange_left_slider.xpm"
#include "colorrange_right_slider.xpm"


enum {
  CHANGED,
  LAST_SIGNAL
};

static gint color_range_signals[LAST_SIGNAL] = { 0 };


static void color_range_calculate_relations(ColorRange *crange) {
  ColorRangeLayout *l;
  gdouble accuracy;

  l = &crange->layout;
  accuracy = crange->accuracy;

  l->scale_width = l->width - l->padding_left - l->padding_right;

  l->unit_width = l->scale_width*accuracy / (1.-accuracy);

  l->padding_top = (l->height - l->scale_height)/2.;
  l->padding_bottom = l->padding_top;

  /*printf("Scale Width: %f\n",l->scale_width);
  printf("Unit Width:  %f\n",l->unit_width);
  printf("Padding Top: %f\n",l->padding_top);*/
}

static void color_range_calculate_minima(ColorRange *crange,gint *_w,gint *_h) {
  ColorRangeLayout *l;
  ColorRangeMinima *m;
  gdouble w,h,accuracy;

  l = &crange->layout;
  m = &crange->minima;
  accuracy = crange->accuracy;

  w = l->padding_left + l->padding_right + (1./accuracy-1.) * m->unit_width;

  h = m->padding_top + m->padding_bottom + l->scale_height;

  if (_w!=NULL) {
    *_w = (gint)w;
  }
  if (_h!=NULL) {
    *_h = (gint)h;
  }
}


static void color_range_draw_scale(ColorRange *crange, cairo_t *cr,gdouble x0,gdouble y0) {
  ColorRangeLayout *l;
  cairo_pattern_t *pat_bg;
  gdouble inset,x;

  l = &crange->layout;

  /* draw background */
  pat_bg = cairo_pattern_create_linear(x0,y0,x0+l->scale_width,y0);
  cairo_pattern_add_color_stop_rgb(pat_bg,0.,0.,0.,0.);
  cairo_pattern_add_color_stop_rgb(pat_bg,1.,crange->color_r,crange->color_g,crange->color_b);
  cairo_set_source(cr,pat_bg);
  cairo_rectangle(cr,x0,y0,l->scale_width,l->scale_height);
  cairo_fill(cr);
  cairo_pattern_destroy(pat_bg);

  /* draw ruler line */
  cairo_set_source_rgb(cr, 5., 5., 5.);
  cairo_set_line_width(cr, 1.);
  cairo_move_to(cr, x0,                y0+l->scale_height/2.);
  cairo_line_to(cr, x0+l->scale_width, y0+l->scale_height/2.);
  cairo_stroke(cr);

  /* draw ticks*/
  for (x=0.;x<=1.;x+=crange->accuracy) {
    inset = (x==0. || x==1.)?l->scale_height:l->tick_length;

    cairo_move_to(cr, x0+x*l->scale_width, y0+l->scale_height/2.-inset/2.);
    cairo_line_to(cr, x0+x*l->scale_width, y0+l->scale_height/2.+inset/2.);
    cairo_stroke(cr);
  }

  /* draw sliders */
  gdk_cairo_set_source_pixbuf(cr, crange->slider_left, x0+crange->value_min*l->scale_width-l->slider_width, y0+l->scale_height/2.-l->slider_height/2.);
  cairo_paint(cr);
  gdk_cairo_set_source_pixbuf(cr, crange->slider_right, x0+crange->value_max*l->scale_width, y0+l->scale_height/2.-l->slider_height/2.);
  cairo_paint(cr);
}

static gboolean color_range_signal_expose_event(GtkWidget *widget,GdkEventExpose *event,ColorRange *crange) {
  ColorRangeLayout *l;
  cairo_t *cr;
  GdkRectangle focus_rect;

  l = &crange->layout;

  //printf("Configure: (%f, %f)\n",w,h);

  /* get cairo context */
  cr = gdk_cairo_create(widget->window);

  /* set a clip region for the expose event */
  cairo_rectangle(cr,event->area.x,event->area.y,event->area.width,event->area.height);
  cairo_clip(cr);

  /* clear background */
  cairo_rectangle(cr,0.,0.,l->width,l->height);
  cairo_set_source_rgb(cr, 5., 5., 5.);
  cairo_fill(cr);

  /* draw focus */
  gtk_paint_focus(widget->style,
                  widget->window,
                  crange->state,
                  &event->area,
                  widget,
                  NULL,
                  (gint)l->padding_left,
                  (gint)l->padding_top,
                  (gint)l->scale_width,
                  (gint)l->scale_height);

  /* draw scale*/
  color_range_draw_scale(crange,cr,l->padding_left,l->padding_top);

  return TRUE;
}

static void color_range_expose(ColorRange *crange) {
  GdkRectangle rect;
  GtkWidget *widget;

  widget = GTK_WIDGET(crange);

  if (GDK_IS_WINDOW(widget->window)) {
    rect.x = 0;
    rect.y = 0;
    rect.width = crange->layout.width;
    rect.height = crange->layout.height;

    gdk_window_invalidate_rect(widget->window,&rect,FALSE);
  }
}

static gboolean color_range_signal_configure_event(GtkWidget *widget,GdkEventConfigure *event,ColorRange *crange) {
  ColorRangeLayout *l;

  l = &crange->layout;

  l->width = (gdouble)event->width;
  l->height = (gdouble)event->height;
  color_range_calculate_relations(crange);

  return TRUE;
}

static gboolean color_range_signal_motion_notify_event(GtkWidget *widget,GdkEventMotion *event,ColorRange *crange) {
  gdouble v;
  ColorRangeLayout *l;

  if (event->state&GDK_BUTTON1_MASK && crange->grabbed!=0) {
    l = &crange->layout;

    v = (event->x-l->padding_left)/l->scale_width;

    if (v<0.) {
      v = 0.;
    }
    if (v>1.) {
      v = 1.;
    }

    if (crange->magnetic) {
      v = round(v/crange->accuracy)*crange->accuracy;
    }

    switch (crange->grabbed) {
      case COLOR_RANGE_LEFT_SLIDER:
        crange->value_min = v<crange->value_max?v:crange->value_max;
        break;
      case COLOR_RANGE_RIGHT_SLIDER:
        crange->value_max = v>crange->value_min?v:crange->value_min;
        break;
    }

    crange->changed = 1;
    color_range_expose(crange);
  }
}

static gboolean color_range_signal_enter_notify_event(GtkWidget *widget,GdkEventCrossing *event,ColorRange *crange) {
  if (crange->state==GTK_STATE_NORMAL) {
    crange->state = GTK_STATE_PRELIGHT;
  }
}

static gboolean color_range_signal_leave_notify_event(GtkWidget *widget,GdkEventCrossing *event,ColorRange *crange) {
  if (crange->state==GTK_STATE_PRELIGHT) {
    crange->state = GTK_STATE_NORMAL;
  }
}

static gboolean color_range_signal_button_release_event(GtkWidget *widget,GdkEventButton *event,ColorRange *crange) {
  crange->grabbed = 0;

  // if changed, emit signal
  if (crange->changed) {
    g_signal_emit(crange,color_range_signals[CHANGED],0,G_TYPE_NONE);
    crange->changed = FALSE;
  }
}

static gboolean color_range_signal_button_press_event(GtkWidget *widget,GdkEventButton *event,ColorRange *crange) {
  gdouble v,dl,dr;
  ColorRangeLayout *l;

  crange->state = GTK_STATE_SELECTED;

  l = &crange->layout;

  v = (event->x-l->padding_left)/l->scale_width;
  dl = fabs(crange->value_min-v);
  dr = fabs(crange->value_max-v);

  if (dl<dr) {
    crange->grabbed = COLOR_RANGE_LEFT_SLIDER;
  }
  else if (dl>dr) {
    crange->grabbed = COLOR_RANGE_RIGHT_SLIDER;
  }
  else {
    crange->grabbed = v>crange->value_min?COLOR_RANGE_RIGHT_SLIDER:COLOR_RANGE_LEFT_SLIDER;
  }
}

static void color_range_signal_realize(GtkWidget *widget,ColorRange *crange) {
  gint min_w,min_h;

  /* set size request */
  color_range_calculate_minima(crange,&min_w,&min_h);
  gtk_widget_set_size_request(widget,min_w,min_h);
}

static void color_range_destroy(GtkObject *object) {
  ColorRange *crange;
  ColorRangeClass *klass;

  g_return_if_fail(object!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(object));
  crange = COLOR_RANGE(object);

  klass = gtk_type_class(gtk_widget_get_type());

  if (GTK_OBJECT_CLASS(klass)->destroy) {
     (* GTK_OBJECT_CLASS(klass)->destroy) (object);
  }

}

static void color_range_class_init(ColorRangeClass *klass) {
  GtkObjectClass *object_class;

  color_range_signals[CHANGED] = g_signal_new("changed",G_TYPE_FROM_CLASS(klass),
                                           G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                                           G_STRUCT_OFFSET(ColorRangeClass,changed),
                                           NULL,NULL,g_cclosure_marshal_VOID__VOID,
                                           G_TYPE_NONE,0);

  object_class = GTK_OBJECT_CLASS(klass);
  object_class->destroy = color_range_destroy;

  klass->changed = NULL;
}

static void color_range_init(ColorRange *crange) {
  GtkWidget *widget;
  gint min_width,min_height;
  ColorRangeLayout *l;
  ColorRangeMinima *m;

  widget = GTK_WIDGET(crange);

  /* set defaults */
  crange->accuracy = .0625; /* 1/16: every 16th color */
  crange->value_min = 0.;
  crange->value_max = 0.;
  crange->color_r = 1.;
  crange->color_g = 1.;
  crange->color_b = 1.;
  crange->grabbed = 0;
  crange->changed = FALSE;
  crange->magnetic = TRUE;

  /* load slider graphics */
  crange->slider_left = gdk_pixbuf_new_from_xpm_data((const char **)color_range_left_slider_xpm);
  crange->slider_right = gdk_pixbuf_new_from_xpm_data((const char **)color_range_right_slider_xpm);

  /* set layout */
  l = &crange->layout;
  l->padding_left = 30.;
  l->padding_right = 30.;
  l->scale_height = 40.;
  l->tick_length = 20.;
  l->slider_width = (gdouble)gdk_pixbuf_get_width(crange->slider_left);
  l->slider_height = (gdouble)gdk_pixbuf_get_height(crange->slider_left);
  crange->state = GTK_STATE_NORMAL;

  /* set fixed minima */
  m = &crange->minima;
  m->unit_width = 20.;
  m->padding_top = 15.;
  m->padding_bottom = 15.;

  /* add events */
  gtk_widget_add_events(widget,GDK_POINTER_MOTION_MASK
                             | GDK_POINTER_MOTION_HINT_MASK
                             | GDK_BUTTON_PRESS_MASK
                             | GDK_BUTTON_RELEASE_MASK
                             | GDK_ENTER_NOTIFY_MASK
                             | GDK_LEAVE_NOTIFY_MASK
                             | GDK_FOCUS_CHANGE_MASK);

  /* connect signals handlers */
  g_signal_connect(G_OBJECT(crange),"realize",G_CALLBACK(color_range_signal_realize),crange);
  g_signal_connect(G_OBJECT(crange),"configure-event",G_CALLBACK(color_range_signal_configure_event),crange);
  g_signal_connect(G_OBJECT(crange),"expose-event",G_CALLBACK(color_range_signal_expose_event),crange);
  g_signal_connect(G_OBJECT(crange),"motion-notify-event",G_CALLBACK(color_range_signal_motion_notify_event),crange);
  g_signal_connect(G_OBJECT(crange),"enter-notify-event",G_CALLBACK(color_range_signal_enter_notify_event),crange);
  g_signal_connect(G_OBJECT(crange),"leave-notify-event",G_CALLBACK(color_range_signal_leave_notify_event),crange);
  g_signal_connect(G_OBJECT(crange),"button-press-event",G_CALLBACK(color_range_signal_button_press_event),crange);
  g_signal_connect(G_OBJECT(crange),"button-release-event",G_CALLBACK(color_range_signal_button_release_event),crange);

}

GType color_range_get_type() {
  static GType crange_type = 0;

  if (crange_type==0) {
    GTypeInfo crange_info = {
      sizeof(ColorRangeClass),
      NULL,
      NULL,
      (GClassInitFunc)color_range_class_init,
      NULL,
      NULL,
      sizeof(ColorRange),
      0,
      (GInstanceInitFunc)color_range_init
    };

    crange_type = g_type_register_static(GTK_TYPE_DRAWING_AREA,"ColorRange",&crange_info,0);
  }

  return crange_type;
}


GtkWidget *color_range_new(gdouble r,gdouble g,gdouble b) {
  ColorRange *crange;

  crange = COLOR_RANGE(g_object_new(COLOR_RANGE_TYPE,NULL));

  crange->color_r = r;
  crange->color_g = g;
  crange->color_b = b;

  return GTK_WIDGET(crange);
}


gdouble color_range_get_accuracy(ColorRange *crange) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  return crange->accuracy;
}

void color_range_set_accuracy(ColorRange *crange,gdouble accuracy) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  crange->accuracy = accuracy;

  color_range_expose(crange);
}

void color_range_get_color(ColorRange *crange,gdouble *r,gdouble *g,gdouble *b) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  if (r!=NULL) {
    *r = crange->color_r;
  }
  if (g!=NULL) {
    *g = crange->color_g;
  }
  if (b!=NULL) {
    *b = crange->color_b;
  }
}

void color_range_set_color(ColorRange *crange,gdouble r,gdouble g,gdouble b) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  crange->color_r = r;
  crange->color_g = g;
  crange->color_b = b;

  color_range_expose(crange);
}

gdouble color_range_get_value_min(ColorRange *crange) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  return crange->value_min;
}

gdouble color_range_get_value_max(ColorRange *crange) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  return crange->value_max;
}

void color_range_set_value_min(ColorRange *crange,gdouble min) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  crange->value_min = min;

  color_range_expose(crange);
}

void color_range_set_value_max(ColorRange *crange,gdouble max) {
  g_return_if_fail(crange!=NULL);
  g_return_if_fail(IS_COLOR_RANGE(crange));

  crange->value_max = max;

  color_range_expose(crange);
}
