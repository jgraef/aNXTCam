/*
    colorpreview.h
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

#ifndef __COLOR_PREVIEW_H__
#define __COLOR_PREVIEW_H__

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtkdrawingarea.h>

G_BEGIN_DECLS


#define COLOR_PREVIEW_TYPE            (color_preview_get_type())
#define COLOR_PREVIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),COLOR_PREVIEW_TYPE,ColorPreview))
#define COLOR_PREVIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),COLOR_PREVIEW_TYPE,ColorPreviewClass))
#define IS_COLOR_PREVIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),COLOR_PREVIEW_TYPE))
#define IS_COLOR_PREVIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),COLOR_PREVIEW_TYPE))

#define COLOR_PREVIEW_LEFT_SLIDER     1
#define COLOR_PREVIEW_RIGHT_SLIDER    2


typedef struct _ColorPreview        ColorPreview;
typedef struct _ColorPreviewClass   ColorPreviewClass;
typedef struct _ColorPreviewColor   ColorPreviewColor;

struct _ColorPreviewColor {
  gdouble r,g,b;
};

struct _ColorPreview {
  GtkDrawingArea drawarea;

  ColorPreviewColor color_low;
  ColorPreviewColor color_high;

  gint min_w;
  gint min_h;
};

struct _ColorPreviewClass {
  GtkDrawingAreaClass parent_class;
};

GType color_preview_get_type(void);
GtkWidget *color_preview_new();

void color_preview_get_color_low(ColorPreview *clpreview,gdouble *r,gdouble *g,gdouble *b);
void color_preview_set_color_low(ColorPreview *clpreview,gdouble r,gdouble g,gdouble b);
void color_preview_get_color_high(ColorPreview *clpreview,gdouble *r,gdouble *g,gdouble *b);
void color_preview_set_color_high(ColorPreview *clpreview,gdouble r,gdouble g,gdouble b);

G_END_DECLS

#endif /* __COLOR_PREVIEW_H__ */
