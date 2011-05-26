/*
    gui/colormap.c
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "nxtcam/gui.h"
#include "nxtcam/colormap.h"
#include "colorrange_widget/colorrange_widget.h"

gdouble midTabRGB[3][8];//global variables of mean RGB values for each tab [R,G,B][tabindex]

static void gui_colormap_update_title(gui_gd_t *gd) {
  char *title;
  char *path;

  if (gd->colormap.current!=NULL) {
    path = strdup(gd->colormap.current);
    asprintf(&title,"aNXTCam - Colors: %s",basename(path));
    free(path);
  }
  else {
    title = strdup("aNXTCam - Colors");
  }

  gtk_window_set_title(GTK_WINDOW(gd->colormap.win),title);

  free(title);
}

static void gui_colormap_gui2data(ColorRangeWidget *crange,nxtcam_colormap_t *colormap) {
  int h,i;
  double j,rl,rh,gl,gh,bl,bh;

  memset(colormap,0,sizeof(nxtcam_colormap_t));

  for (h=0;h<8;h++) {
    color_range_widget_get_color_low(crange,h,&rl,&gl,&bl);
    color_range_widget_get_color_high(crange,h,&rh,&gh,&bh);

	//store the median of each RGB value for each tab
	if (rh == 0 && rl == 0) midTabRGB[0][h] = NAN;
	else midTabRGB[0][h] = (rh+rl)*255/2;
	if (gh == 0 && gl == 0) midTabRGB[1][h] = NAN;
	else midTabRGB[1][h] = (gh+gl)*255/2;
	if (bh == 0 && bl == 0) midTabRGB[2][h] = NAN;
	else midTabRGB[2][h] = (bh+bl)*255/2;

    for (i=0,j=0.;i<16;i++,j+=.0625) {
      if (j>=rl && j<rh) {
        colormap->r[i] |= (0x80>>h);
      }
      if (j>=gl && j<gh) {
        colormap->g[i] |= (0x80>>h);
      }
      if (j>=bl && j<bh) {
        colormap->b[i] |= (0x80>>h);
      }
    }
  }
}

static void gui_colormap_data2gui(ColorRangeWidget *crange,nxtcam_colormap_t *colormap) {
  int h,i;
  double j;
  double rl,rh,gl,gh,bl,bh;

  color_range_widget_clearall(crange);

  for (h=0;h<8;h++) {
    rl = NAN;
    rh = 0.;
    gl = NAN;
    gh = 0.;
    bl = NAN;
    bh = 0.;

    for (i=0,j=0.;i<16;i++,j+=.0625) {
      if (colormap->r[i]&(0x80>>h)) {
        if (isnan(rl)) {
          rl = j;
        }
        rh = j+.0625;
      }

      if (colormap->g[i]&(0x80>>h)) {
        if (isnan(gl)) {
          gl = j;
        }
        gh = j+.0625;
      }

      if (colormap->b[i]&(0x80>>h)) {
        if (isnan(bl)) {
          bl = j;
        }
        bh = j+.0625;
      }
    }


    rl = isnan(rl)?0.:rl;
    gl = isnan(gl)?0.:gl;
    bl = isnan(bl)?0.:bl;

    color_range_widget_set_color_low(crange,h,rl,gl,bl);
    color_range_widget_set_color_high(crange,h,rh,gh,bh);
  }
}

void gui_colormap_autosave(gui_gd_t *gd) {
  nxtcam_colormap_t colormap;

  gui_colormap_gui2data(COLOR_RANGE_WIDGET(gd->colormap.colorrange),&colormap);
  nxtcam_colormap_save("autosave.clm",&colormap);
}

void gui_colormap_autoload(gui_gd_t *gd) {
  nxtcam_colormap_t colormap;

  if (nxtcam_colormap_open("autosave.clm",&colormap)==0) {
    gui_colormap_data2gui(COLOR_RANGE_WIDGET(gd->colormap.colorrange),&colormap);
  }
}

/*
**Check for overlapping colors in all 8 tabs
*/
gboolean pixelOverlap(gui_gd_t *gd){
	nxtcam_colormap_t colormap;

	gui_colormap_gui2data(COLOR_RANGE_WIDGET(gd->colormap.colorrange),&colormap);
	gtk_label_set_text(G_OBJECT(gd->colormap.alertOverlap),NULL);

	int i = 0;
	gboolean overlapExists = FALSE;
	gdouble distance[8][8];
	int count = 0;
	for (i; i<8; i++){
		int k = i+1;
		for (k; k<8; k++){
			distance[i][k] = sqrt(pow((midTabRGB[0][i]-midTabRGB[0][k]),2) + pow((midTabRGB[1][i]-midTabRGB[1][k]),2) +pow((midTabRGB[2][i]-midTabRGB[2][k]),2)); //Distance formula to calculate the distance between each point with the other
			if (distance[i][k] <= 25) {
				char *overlaps;
				asprintf(&overlaps,"Overlapping Colors: %d & %d ",i+1,k+1);
				gtk_label_set_text(G_OBJECT(gd->colormap.alertOverlap),overlaps);
				free (overlaps);
				overlapExists = TRUE;
			}
		}
	}

	return overlapExists;
}

void gui_colormap_upload(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_colormap_t colormap;

  if (nxtcam_com_is_connected() && !pixelOverlap(gd)) { //if NXTCam is connected and there is no overlap
   gui_colormap_gui2data(COLOR_RANGE_WIDGET(gd->colormap.colorrange),&colormap);
    nxtcam_dev_req_colormap(&colormap);
  }
  else if (nxtcam_com_is_connected() && pixelOverlap(gd)){ //if NXTCam is connected and there is overlap

  }
  else {
    gui_error(gd,"Could not upload colors","NXTCam not connected");
  }
}

G_MODULE_EXPORT void gui_colormap_new(GtkWidget *widget,gui_gd_t *gd) {
  // reset current file
  free(gd->colormap.current);
  gd->colormap.current = NULL;
  gui_colormap_update_title(gd);

  // clear colormap widget
  color_range_widget_clearall(COLOR_RANGE_WIDGET(gd->colormap.colorrange));
}

G_MODULE_EXPORT void gui_colormap_open(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_colormap_t colormap;
  GtkWidget *dialog;
  char *filename;

  dialog = gtk_file_chooser_dialog_new("Open...",GTK_WINDOW(gd->frame.win),GTK_FILE_CHOOSER_ACTION_OPEN,
                                       "gtk-cancel",GTK_RESPONSE_CANCEL,
                                       "gtk-open",GTK_RESPONSE_ACCEPT,
                                       NULL);
  gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog),0);
  gui_file_filter_add_colormap(GTK_FILE_CHOOSER(dialog));

  if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

    if (filename!=NULL) {
      // open color map
      if (nxtcam_colormap_open(filename,&colormap)==-1) {
        gui_error(gd,"Could not open color map","\"%s\"",filename);
      }
      else {
        gui_colormap_data2gui(COLOR_RANGE_WIDGET(gd->colormap.colorrange),&colormap);
      }

      // set name of current file and update window title
      free(gd->colormap.current);
      gd->colormap.current = filename;
      gui_colormap_update_title(gd);
    }
  }

  gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void gui_colormap_saveas(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_colormap_t colormap;
  GtkWidget *dialog;
  char *filename;

  dialog = gtk_file_chooser_dialog_new("Save as...",GTK_WINDOW(gd->frame.win),GTK_FILE_CHOOSER_ACTION_SAVE,
                                       "gtk-cancel",GTK_RESPONSE_CANCEL,
                                       "gtk-save",GTK_RESPONSE_ACCEPT,
                                       NULL);
  gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog),0);
  gui_file_filter_add_colormap(GTK_FILE_CHOOSER(dialog));

  if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

    if (filename!=NULL) {
      // save color map
      gui_colormap_gui2data(COLOR_RANGE_WIDGET(gd->colormap.colorrange),&colormap);
      if (nxtcam_colormap_save(filename,&colormap)==-1) {
        gui_error(gd,"Could not save color map","\"%s\"",filename);
      }

      // set name of current file and update window title
      free(gd->colormap.current);
      gd->colormap.current = filename;
      gui_colormap_update_title(gd);
    }
  }

  gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void gui_colormap_save(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_colormap_t colormap;

  if (gd->colormap.current==NULL) {
    // if no colormap opened, do "save as"
    gui_colormap_saveas(widget,gd);
  }
  else {
    // save colormap
    gui_colormap_gui2data(COLOR_RANGE_WIDGET(gd->colormap.colorrange),&colormap);
    if (nxtcam_colormap_save(gd->colormap.current,&colormap)==-1) {
      gui_error(gd,"Could not save color map","\"%s\"",gd->colormap.current);
    }
  }
}

G_MODULE_EXPORT void gui_colormap_close(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_hide(gd->colormap.win);
}
