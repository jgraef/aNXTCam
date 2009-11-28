/*
    colorrange_widget.c
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

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "colorrange/colorrange.h"
#include "colorpreview/colorpreview.h"
#include "colorrange_widget/colorrange_widget.h"


typedef struct {
  gdouble x;
  gdouble y;
  gdouble w;
  gdouble h;
} ColorRangeWidgetArea;

enum {
  UPLOAD,
  LAST_SIGNAL
};

static gint color_range_widget_signals[LAST_SIGNAL] = { 0 };


static void color_range_widget_button_clear(GtkWidget *widget,ColorRangeWidget *crangewidget) {
  gint tabnum;

  tabnum = color_range_get_current_tab(crangewidget);
  color_range_widget_clear(crangewidget,tabnum);
}

static void color_range_widget_button_clearall(GtkWidget *widget,ColorRangeWidget *crangewidget) {
  color_range_widget_clearall(crangewidget);
}

static void color_range_widget_button_upload(GtkWidget *widget,ColorRangeWidget *crangewidget) {
  g_signal_emit(crangewidget,color_range_widget_signals[UPLOAD],0,G_TYPE_NONE);
}

static void color_range_widget_signal_changed(GtkWidget *widget,ColorRangeWidgetTab *tab) {
  gdouble r,g,b;

  r = color_range_get_value_min(tab->crange_red);
  g = color_range_get_value_min(tab->crange_green);
  b = color_range_get_value_min(tab->crange_blue);
  color_preview_set_color_low(tab->clpreview,r,g,b);

  r = color_range_get_value_max(tab->crange_red);
  g = color_range_get_value_max(tab->crange_green);
  b = color_range_get_value_max(tab->crange_blue);
  color_preview_set_color_high(tab->clpreview,r,g,b);
  }

static void color_range_widget_destroy(GtkObject *object) {
  ColorRangeWidget *crangewidget;
  ColorRangeWidgetClass *klass;

  g_return_if_fail(object!=NULL);
  g_return_if_fail(IS_COLOR_RANGE_WIDGET(object));
  crangewidget = COLOR_RANGE_WIDGET(object);

  klass = gtk_type_class(gtk_widget_get_type());

  if (GTK_OBJECT_CLASS(klass)->destroy) {
     (* GTK_OBJECT_CLASS(klass)->destroy) (object);
  }

}

static void color_range_widget_class_init(ColorRangeWidgetClass *klass) {
  GtkObjectClass *object_class;

  color_range_widget_signals[UPLOAD] = g_signal_new("upload",G_TYPE_FROM_CLASS(klass),
                                           G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                                           G_STRUCT_OFFSET(ColorRangeWidgetClass,upload),
                                           NULL,NULL,g_cclosure_marshal_VOID__VOID,
                                           G_TYPE_NONE,0);
  klass->upload = NULL;

  object_class = GTK_OBJECT_CLASS(klass);
  object_class->destroy = color_range_widget_destroy;
}

static void color_range_widget_init(ColorRangeWidget *crangewidget) {
  GtkWidget *box, *label, *range, *btnbox, *clpreview;
  GtkWidget *btn_clear, *btn_clearall, *btn_upload;
  gint i;

  crangewidget->notebook = gtk_notebook_new();
  btnbox = gtk_hbutton_box_new();

  btn_clear = gtk_button_new_with_label("Clear");
  g_signal_connect(G_OBJECT(btn_clear),"clicked",G_CALLBACK(color_range_widget_button_clear),crangewidget);
  gtk_box_pack_start(GTK_BOX(btnbox),btn_clear,0,1,0);

  btn_clearall = gtk_button_new_with_label("Clear All");
  g_signal_connect(G_OBJECT(btn_clearall),"clicked",G_CALLBACK(color_range_widget_button_clearall),crangewidget);
  gtk_box_pack_start(GTK_BOX(btnbox),btn_clearall,0,1,0);

  btn_upload = gtk_button_new_with_label("Upload");
  g_signal_connect(G_OBJECT(btn_upload),"clicked",G_CALLBACK(color_range_widget_button_upload),crangewidget);
  gtk_box_pack_start(GTK_BOX(btnbox),btn_upload,0,1,0);

  for (i=0;i<COLOR_RANGE_WIDGET_NUM_TABS;i++) {
    box = gtk_vbox_new(1,0);

    clpreview = color_preview_new();
    crangewidget->tabs[i].clpreview = COLOR_PREVIEW(clpreview);

    range = color_range_new(1.,0.,0.);
    crangewidget->tabs[i].crange_red = COLOR_RANGE(range);
    g_signal_connect(G_OBJECT(range),"changed",G_CALLBACK(color_range_widget_signal_changed),crangewidget->tabs+i);
    gtk_box_pack_start(GTK_BOX(box),range,0,1,0);

    range = color_range_new(0.,1.,0.);
    crangewidget->tabs[i].crange_green = COLOR_RANGE(range);
    g_signal_connect(G_OBJECT(range),"changed",G_CALLBACK(color_range_widget_signal_changed),crangewidget->tabs+i);
    gtk_box_pack_start(GTK_BOX(box),range,0,1,0);

    range = color_range_new(0.,0.,1.);
    crangewidget->tabs[i].crange_blue = COLOR_RANGE(range);
    g_signal_connect(G_OBJECT(range),"changed",G_CALLBACK(color_range_widget_signal_changed),crangewidget->tabs+i);
    gtk_box_pack_start(GTK_BOX(box),range,0,1,0);

    gtk_notebook_append_page(GTK_NOTEBOOK(crangewidget->notebook),box,clpreview);
  }

  gtk_box_pack_start(GTK_BOX(crangewidget),crangewidget->notebook,1,1,0);
  gtk_box_pack_start(GTK_BOX(crangewidget),btnbox,1,1,0);
  gtk_widget_show_all(crangewidget->notebook);
  gtk_widget_show_all(btnbox);
}

GType color_range_widget_get_type() {
  static GType crangewidget_type = 0;

  if (crangewidget_type==0) {
    GTypeInfo crangewidget_info = {
      sizeof(ColorRangeWidgetClass),
      NULL,
      NULL,
      (GClassInitFunc)color_range_widget_class_init,
      NULL,
      NULL,
      sizeof(ColorRangeWidget),
      0,
      (GInstanceInitFunc)color_range_widget_init
    };

    crangewidget_type = g_type_register_static(GTK_TYPE_VBOX,"ColorRangeWidget",&crangewidget_info,0);
  }

  return crangewidget_type;
}


GtkWidget *color_range_widget_new() {
  ColorRangeWidget *crangewidget;

  crangewidget = COLOR_RANGE_WIDGET(g_object_new(COLOR_RANGE_WIDGET_TYPE,NULL));

  return GTK_WIDGET(crangewidget);
}

void color_range_widget_get_color_low(ColorRangeWidget *crangewidget,gint tabnum,gdouble *r,gdouble *g,gdouble *b) {
  ColorRangeWidgetTab *tab;

  g_return_if_fail(crangewidget!=NULL);
  g_return_if_fail(IS_COLOR_RANGE_WIDGET(crangewidget));
  g_return_if_fail(tabnum>=0 && tabnum<COLOR_RANGE_WIDGET_NUM_TABS);

  tab = crangewidget->tabs+tabnum;

  if (r!=NULL) {
    *r = color_range_get_value_min(tab->crange_red);
  }
  if (g!=NULL) {
    *g = color_range_get_value_min(tab->crange_green);
  }
  if (b!=NULL) {
    *b = color_range_get_value_min(tab->crange_blue);
  }

}

void color_range_widget_set_color_low(ColorRangeWidget *crangewidget,gint tabnum,gdouble r,gdouble g,gdouble b) {
  ColorRangeWidgetTab *tab;

  g_return_if_fail(crangewidget!=NULL);
  g_return_if_fail(IS_COLOR_RANGE_WIDGET(crangewidget));
  g_return_if_fail(tabnum>=0 && tabnum<COLOR_RANGE_WIDGET_NUM_TABS);

  tab = crangewidget->tabs+tabnum;

  color_range_set_value_min(tab->crange_red,r);
  color_range_set_value_min(tab->crange_green,g);
  color_range_set_value_min(tab->crange_blue,b);

  color_range_widget_signal_changed(GTK_WIDGET(crangewidget),tab);
}

void color_range_widget_get_color_high(ColorRangeWidget *crangewidget,gint tabnum,gdouble *r,gdouble *g,gdouble *b) {
  ColorRangeWidgetTab *tab;

  g_return_if_fail(crangewidget!=NULL);
  g_return_if_fail(IS_COLOR_RANGE_WIDGET(crangewidget));
  g_return_if_fail(tabnum>=0 && tabnum<COLOR_RANGE_WIDGET_NUM_TABS);

  tab = crangewidget->tabs+tabnum;

  if (r!=NULL) {
    *r = color_range_get_value_max(tab->crange_red);
  }
  if (g!=NULL) {
    *g = color_range_get_value_max(tab->crange_green);
  }
  if (b!=NULL) {
    *b = color_range_get_value_max(tab->crange_blue);
  }
}

void color_range_widget_set_color_high(ColorRangeWidget *crangewidget,gint tabnum,gdouble r,gdouble g,gdouble b) {
  ColorRangeWidgetTab *tab;

  g_return_if_fail(crangewidget!=NULL);
  g_return_if_fail(IS_COLOR_RANGE_WIDGET(crangewidget));
  g_return_if_fail(tabnum>=0 && tabnum<COLOR_RANGE_WIDGET_NUM_TABS);

  tab = crangewidget->tabs+tabnum;

  color_range_set_value_max(tab->crange_red,r);
  color_range_set_value_max(tab->crange_green,g);
  color_range_set_value_max(tab->crange_blue,b);

  color_range_widget_signal_changed(GTK_WIDGET(crangewidget),tab);
}

void color_range_widget_clear(ColorRangeWidget *crangewidget,gint tabnum) {
  color_range_widget_set_color_low(crangewidget,tabnum,0.,0.,0.);
  color_range_widget_set_color_high(crangewidget,tabnum,0.,0.,0.);
}

void color_range_widget_clearall(ColorRangeWidget *crangewidget) {
  gint i;

  for (i=0;i<COLOR_RANGE_WIDGET_NUM_TABS;i++) {
    color_range_widget_clear(crangewidget,i);
  }
}

gint color_range_get_current_tab(ColorRangeWidget *crangewidget) {
  return gtk_notebook_get_current_page(GTK_NOTEBOOK(crangewidget->notebook));
}
