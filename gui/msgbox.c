/*
    gui/msgbox.c
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
#include <stdarg.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "nxtcam/gui.h"

int gui_msgbox(gui_gd_t *gd,GtkMessageType type,GtkButtonsType buttons,const char *title_fmt,const char *text_fmt,...) {
  char *title;
  char *text;
  GtkWidget *dialog;
  va_list args;
  int ret;

  va_start(args,text_fmt);
  vasprintf(&title,title_fmt,args);
  vasprintf(&text,text_fmt,args);

  dialog = gtk_message_dialog_new(GTK_WINDOW(gd->main.win),GTK_DIALOG_MODAL,type,buttons,title);
  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),text);

  free(text);
  free(title);
  va_end(args);

  ret = gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);

  return ret;
}
