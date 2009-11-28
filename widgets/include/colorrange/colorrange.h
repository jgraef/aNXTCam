/*
    colorrange.h
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

#ifndef __COLOR_RANGE_H__
#define __COLOR_RANGE_H__

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtkdrawingarea.h>

G_BEGIN_DECLS


#define COLOR_RANGE_TYPE            (color_range_get_type())
#define COLOR_RANGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),COLOR_RANGE_TYPE,ColorRange))
#define COLOR_RANGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),COLOR_RANGE_TYPE,ColorRangeClass))
#define IS_COLOR_RANGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),COLOR_RANGE_TYPE))
#define IS_COLOR_RANGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),COLOR_RANGE_TYPE))

#define COLOR_RANGE_LEFT_SLIDER     1
#define COLOR_RANGE_RIGHT_SLIDER    2


typedef struct _ColorRange        ColorRange;
typedef struct _ColorRangeClass   ColorRangeClass;
typedef struct _ColorRangeLayout  ColorRangeLayout;
typedef struct _ColorRangeMinima ColorRangeMinima;

struct _ColorRangeLayout {
  // Fixed
  gdouble padding_left;
  gdouble padding_right;
  gdouble scale_height;
  gdouble tick_length;
  gdouble slider_width;
  gdouble slider_height;

  // Dependent
  gdouble padding_top;
  gdouble padding_bottom;
  gdouble scale_width;
  gdouble unit_width;
  gdouble width;
  gdouble height;
};

struct _ColorRangeMinima {
  gdouble unit_width;
  gdouble padding_top;
  gdouble padding_bottom;
};

struct _ColorRange {
  GtkDrawingArea drawarea;

  gdouble accuracy;

  gdouble value_min;
  gdouble value_max;

  gdouble color_r;
  gdouble color_g;
  gdouble color_b;

  ColorRangeLayout layout;
  ColorRangeMinima minima;

  GdkPixbuf *slider_left;
  GdkPixbuf *slider_right;

  GtkStateType state;
  gint grabbed;
  gboolean changed;
  gboolean magnetic;
};

struct _ColorRangeClass {
  GtkDrawingAreaClass parent_class;

  /* Signals */
  void (*changed)(ColorRange *crange);
};

GType color_range_get_type(void);
GtkWidget *color_range_new(gdouble r,gdouble g,gdouble b);

gdouble color_range_get_accuracy(ColorRange *crange);
void color_range_set_accuracy(ColorRange *crange,gdouble accuracy);
void color_range_get_color(ColorRange *crange,gdouble *r,gdouble *g,gdouble *b);
void color_range_set_color(ColorRange *crange,gdouble r,gdouble g,gdouble b);
gdouble color_range_get_value_min(ColorRange *crange);
gdouble color_range_get_value_max(ColorRange *crange);
void color_range_set_value_min(ColorRange *crange,gdouble min);
void color_range_set_value_max(ColorRange *crange,gdouble max);

G_END_DECLS

#endif /* __COLOR_RANGE_H__ */
