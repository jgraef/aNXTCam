/*
    colorrange_widget.h
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

#ifndef __COLOR_RANGE_WIDGET_H__
#define __COLOR_RANGE_WIDGET_H__

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "colorrange/colorrange.h"
#include "colorpreview/colorpreview.h"

G_BEGIN_DECLS


#define COLOR_RANGE_WIDGET_TYPE            (color_range_widget_get_type())
#define COLOR_RANGE_WIDGET(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),COLOR_RANGE_WIDGET_TYPE,ColorRangeWidget))
#define COLOR_RANGE_WIDGET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),COLOR_RANGE_WIDGET_TYPE,ColorRangeWidgetClass))
#define IS_COLOR_RANGE_WIDGET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),COLOR_RANGE_WIDGET_TYPE))
#define IS_COLOR_RANGE_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),COLOR_RANGE_WIDGET_TYPE))

#define COLOR_RANGE_WIDGET_NUM_TABS 8

typedef struct _ColorRangeWidget        ColorRangeWidget;
typedef struct _ColorRangeWidgetClass   ColorRangeWidgetClass;
typedef struct _ColorRangeWidgetTab     ColorRangeWidgetTab;

struct _ColorRangeWidgetTab {
  ColorRange *crange_red;
  ColorRange *crange_green;
  ColorRange *crange_blue;
  ColorPreview *clpreview;
};

struct _ColorRangeWidget {
  GtkVBox vbox;

  GtkWidget *notebook;

  ColorRangeWidgetTab tabs[COLOR_RANGE_WIDGET_NUM_TABS];
};

struct _ColorRangeWidgetClass {
  GtkVBoxClass parent_class;

  /* Signals */
  void (*upload)(ColorRangeWidget *crangewidget);
};

GType color_range_widget_get_type(void);
GtkWidget *color_range_widget_new();

void color_range_widget_get_color_low(ColorRangeWidget *crangewidget,gint tabnum,gdouble *r,gdouble *g,gdouble *b);
void color_range_widget_set_color_low(ColorRangeWidget *crangewidget,gint tabnum,gdouble r,gdouble g,gdouble b);
void color_range_widget_get_color_high(ColorRangeWidget *crangewidget,gint tabnum,gdouble *r,gdouble *g,gdouble *b);
void color_range_widget_set_color_high(ColorRangeWidget *crangewidget,gint tabnum,gdouble r,gdouble g,gdouble b);
void color_range_widget_clear(ColorRangeWidget *crangewidget,gint tabnum);
void color_range_widget_clearall(ColorRangeWidget *crangewidget);
gint color_range_get_current_tab(ColorRangeWidget *crangewidget);

G_END_DECLS

#endif /* __COLOR_RANGE_WIDGET_H__ */
