/*
    gui/preferences.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "nxtcam/gui.h"
#include "nxtcam/os.h"
#include "iniparser.h"

static int gui_preferences_save(const char *filename,struct gui_preferences *preferences) {
  FILE *fd;

  fd = fopen(filename,"w");

  if (fd!=NULL) {
    fprintf(fd,"[local]\n");
    fprintf(fd,"device = \"%s\"\n\n",preferences->device);

    fprintf(fd,"[nxtcam]\n");
    fprintf(fd,"auto_white_balance       = %s\n",preferences->auto_white_balance?"yes":"no");
    fprintf(fd,"auto_adjust_mode         = %s\n",preferences->auto_adjust_mode?"yes":"no");
    fprintf(fd,"flourescent_light_filter = %s\n",preferences->flourescent_light_filter?"yes":"no");

    fclose(fd);
    return 0;
  }
  else {
    return -1;
  }
}

int gui_preferences_open(const char *filename,struct gui_preferences *preferences) {
  static struct gui_preferences default_preferences = {
    .auto_white_balance = 0,
    .auto_adjust_mode = 0,
    .flourescent_light_filter = 0,
    .device = NXTCAM_OS_DEFAULT_DEVICE
  };
  dictionary *ini;

  ini = iniparser_load(filename);
  if (ini!=NULL) {
    preferences->auto_white_balance = iniparser_getboolean(ini,"nxtcam:auto_white_balance",default_preferences.auto_white_balance);
    preferences->auto_adjust_mode = iniparser_getboolean(ini,"nxtcam:auto_adjust_mode",default_preferences.auto_adjust_mode);
    preferences->flourescent_light_filter = iniparser_getboolean(ini,"nxtcam:flourescent_light_filter",default_preferences.flourescent_light_filter);
    preferences->device = strdup(iniparser_getstring(ini,"local:device",default_preferences.device));
  }
  else {
    memcpy(preferences,&default_preferences,sizeof(struct gui_preferences));
    preferences->device = strdup(default_preferences.device);
    printf("Default device: %s\n",default_preferences.device);
  }

  // always return 0, because if file does not exist, we use default
  return 0;
}

static int gui_preferences_upload(struct gui_preferences *preferences) {
  /*********************************************
  * Register function        * reg * on * off *
  *********************************************
  * auto white balance       *  45 *  7 *   3 *
  * auto adjust mode         *  19 *  1 *   0 *
  * flourescent light filter *  18 * 44 *  40 *
  *********************************************/

  if (nxtcam_com_is_connected()) {
    if (nxtcam_dev_req_regw(45,preferences->auto_white_balance?7:3)==-1) {
      return -1;
    }
    if (nxtcam_dev_req_regw(19,preferences->auto_adjust_mode?1:0)==-1) {
      return -1;
    }
    if (nxtcam_dev_req_regw(18,preferences->flourescent_light_filter?44:40)==-1) {
      return -1;
    }

    return 0;
  }
  else {
    return -1;
  }
}

static void gui_preferences_gui2data(gui_gd_t *gd,struct gui_preferences *preferences) {
  preferences->auto_white_balance = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gd->preferences.chkbtn_whitebalance));
  preferences->auto_adjust_mode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gd->preferences.chkbtn_adjustmode));
  preferences->flourescent_light_filter = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gd->preferences.chkbtn_lightfilter));
  free(preferences->device);
  preferences->device = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(gd->preferences.btn_device));
}

static void gui_preferences_data2gui(gui_gd_t *gd,struct gui_preferences *preferences) {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gd->preferences.chkbtn_whitebalance),preferences->auto_white_balance);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gd->preferences.chkbtn_adjustmode),preferences->auto_adjust_mode);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gd->preferences.chkbtn_lightfilter),preferences->flourescent_light_filter);
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(gd->preferences.btn_device),preferences->device);
}

void gui_preferences_init(gui_gd_t *gd) {
  gui_preferences_open("config.ini",&gd->preferences.current);
  gui_preferences_data2gui(gd,&gd->preferences.current);
}

G_MODULE_EXPORT void gui_preferences_apply(GtkWidget *widget,gui_gd_t *gd) {
  gui_preferences_gui2data(gd,&gd->preferences.current);

  if (gui_preferences_save("config.ini",&gd->preferences.current)==-1) {
    gui_error(gd,"Saving","Could not save preferences");
  }

  if (gui_preferences_upload(&gd->preferences.current)==-1) {
    if (nxtcam_com_is_connected()) {
      gui_error(gd,"Uploading","Can not upload preferences to NXTCam.");
    }
    else {
      gui_warning(gd,"Not connected","NXTCam is not connected, so preferences are not uploaded.");
    }
  }
}

G_MODULE_EXPORT void gui_preferences_cancel(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_hide(gd->preferences.win);
  gui_preferences_data2gui(gd,&gd->preferences.current);
}

G_MODULE_EXPORT void gui_preferences_ok(GtkWidget *widget,gui_gd_t *gd) {
  gui_preferences_apply(widget,gd);
  gui_preferences_cancel(widget,gd);
}

G_MODULE_EXPORT void gui_preferences_reset(GtkWidget *widget,gui_gd_t *gd) {
  GtkWidget *dialog;
  GtkWidget *content_area;
  GtkWidget *label;

  // FIXME create yes/no dialog
  dialog = gtk_dialog_new_with_buttons("Are you sure?",GTK_WINDOW(gd->main.win),
                                       GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_STOCK_YES,GTK_RESPONSE_YES,
                                       GTK_STOCK_NO,GTK_RESPONSE_NO,
                                       NULL);
  content_area = GTK_DIALOG(dialog)->vbox;
  label = gtk_label_new("This will reset your NXTCam to factory default. Do you really want to continue?");
  gtk_container_add(GTK_CONTAINER(content_area),label);

  if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES) {
    // reset NXTCam
    /*nxtcam_dev_req_reset();

    // reset internal preferences
    gd->preferences.current.auto_white_balance = 0;
    gd->preferences.current.auto_adjust_mode = 0;
    gd->preferences.current.flourescent_light_filter = 0;
    gui_preferences_data2gui(gd,&gd->preferences.current);*/
    printf("RESET\n");
  }

  // destroy dialog
  gtk_widget_destroy(dialog);
}
