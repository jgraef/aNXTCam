/*
    gui/main.c
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
#include <stdlib.h>

#include <glib.h>
#include <gtk/gtk.h>

#include "nxtcam/log.h"
#include "nxtcam/gui.h"
#include "nxtcam/com.h"
#include "nxtcam/device.h"
#include "nxtcam/os.h"
#include "nxtcam/frame.h" //include frame.h to reference constants
#include "colorrange_widget/colorrange_widget.h"//include header to reference functions

// Custom widgets
#include "colorrange_widget/colorrange_widget.h"

// Convenience macros for obtaining objects from UI file
#define GUI_GET_OBJECT(builder,name,type) (type(gtk_builder_get_object(builder,name)))
#define GUI_GET_WIDGET(builder,name)      GUI_GET_OBJECT(builder,name,GTK_WIDGET)

/**
 * Convenience function for changing background color of a widget
 *  @param widget Widget to change bg color of
 *  @param spec Color specification string (e.g "red" or "#ff0000")
 */
static void gui_widget_bgcolor(GtkWidget *widget,const char *spec) {
  GdkColormap *colormap;
  GdkColor color;

  colormap = gtk_widget_get_colormap(widget);
  gdk_color_parse(spec,&color);
  gdk_colormap_alloc_color(colormap,&color,0,1);

  gtk_widget_modify_bg(widget,GTK_STATE_NORMAL,&color);
  gtk_widget_modify_bg(widget,GTK_STATE_PRELIGHT,&color);
  gtk_widget_modify_bg(widget,GTK_STATE_ACTIVE,&color);
}

/*
 * Callbacks for signals from main window
 */

G_MODULE_EXPORT void gui_main_connect(GtkWidget *widget,gui_gd_t *gd) {
  const char *device;

  if (!nxtcam_com_is_connected()) {
    device = gd->preferences.current.device;
    logmsg("Using device: %s\n",device);

    if (nxtcam_com_open(device)!=-1) {
      nxtcam_dev_flush();

      // set tracking to default
      nxtcam_dev_req_tracking(0);
      nxtcam_dev_req_tracking_type(NXTCAM_TRACK_OBJECT);
    }
    else {
      logmsg("Could not open COM device\n");
    }
  }
  else {
    logmsg("NXTCam already connected\n");
  }
}

G_MODULE_EXPORT void gui_main_disconnect(GtkWidget *widget,gui_gd_t *gd) {
  if (nxtcam_com_is_connected()) {
    // disconnect NXTCam
    nxtcam_com_close();
  }
  else {
    logmsg("NXTCam not connected\n");
  }
}

G_MODULE_EXPORT void gui_main_getversion(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_show(gd->version.win);
  gui_version_query(widget,gd);
}

G_MODULE_EXPORT void gui_main_ping(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_show(gd->ping.win);
  gui_ping_ping(widget,gd);
}

G_MODULE_EXPORT void gui_main_capture(GtkWidget *widget,gui_gd_t *gd) {
  if (nxtcam_com_is_connected()) {
	gtk_widget_show(gd->frame.win);
	gui_frame_capture(widget,gd);  }
  else {
    logmsg("NXTCam not connected\n");
  }
}

G_MODULE_EXPORT void gui_main_abort(GtkWidget *widget,gui_gd_t *gd) {
  nxtcam_dev_abort();
}

G_MODULE_EXPORT void gui_main_preferences(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_show(gd->preferences.win);
}

G_MODULE_EXPORT void gui_main_colormap(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_show(gd->colormap.win);
}

G_MODULE_EXPORT void gui_main_tracking(GtkWidget *widget,gui_gd_t *gd) {
  if (nxtcam_com_is_connected()) {
    gtk_widget_show(gd->tracking.win);
  }
  else {
    logmsg("NXTCam not connected\n");
  }
}

G_MODULE_EXPORT void gui_main_about(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_show(gd->about.dialog);
}

G_MODULE_EXPORT void gui_main_close(GtkWidget *widget,gui_gd_t *gd) {
  gtk_widget_destroy(gd->main.win);
}

G_MODULE_EXPORT void gui_main_destroy(GtkWidget *widget,GdkEvent *event,gui_gd_t *gd) {
  gd->sync = 0;
  gui_colormap_autosave(gd);
  gtk_main_quit();
}

/**
 * Syncs GUI with NXTCam
 *  @param gd GUI descriptor
 *  @return Returns false if it should be removed from timer list
 */
static gboolean gui_sync(gpointer data) {
  gui_gd_t *gd;
  int connected;

  gd = (gui_gd_t*)data;

  if (gd->sync) {
    connected = nxtcam_com_is_connected();

    // sync statusbar
    gui_statusbar_msg(gd,connected?"Connected":"Disconnected");
    gui_statusbar_led(gd,connected);

    // sync frame window
    gui_frame_update(gd);

    // sync tracking
    gui_tracking_update(gd);

    // sync version
    gui_version_update(gd);

    // sync ping
    gui_ping_update(gd);
  }

  return 1;
}

/**
 *Function for the Button Press Event on the image capture frame window
**/
static gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data){
	//pointer declarations
	gui_gd_t *gd;
	gd = (gui_gd_t*)data;
	nxtcam_colormap_t colormap;
	
	gint w = gd->frame.img_frame->allocation.width;
	gint h = gd->frame.img_frame->allocation.height;

	//Create the variable for the drawable dimensions of the image frame	
	gint winFrameWidth;
	gint winFrameHeight;
	//Get the "drawable" size of the image frame window and store the dimensions in the two global variables declared in the frame.h file
	gdk_drawable_get_size(gtk_widget_get_window(gd->frame.img_frame),&winFrameWidth,&winFrameHeight);

	//calculated offsets take into acount the displacement from origin of the captured image
	gint xOffset = gd->frame.img_frame->allocation.x;
	gint yOffset = gd->frame.img_frame->allocation.y;
	
	//the corrected x,y coordinates of the point click with respect to the image stored in the pixelArray(s)
	gint x = ((event->x)-xOffset)*NXTCAM_FRAME_WIDTH/w;
	gint y = ((event->y)-yOffset)*NXTCAM_FRAME_HEIGHT/h;	

	//Button check to specify that only the left click button is used and the actions are only executed if the click happens within the captured image
	if (event->button == 1 && x <= NXTCAM_FRAME_WIDTH && x >= 0 && y <= NXTCAM_FRAME_HEIGHT && y >= 0) {
		//obtain the RGB values at the x,y coordinates of the click
		gint red = pixelArrayR[x][y];
		gint green = pixelArrayG[x][y];
		gint blue = pixelArrayB[x][y];

		//obtain the tolerance from the spin-button in colormap.win
		gdouble tol = gtk_spin_button_get_value (gd->colormap.tolerance)/200;

		//calculate and store the low and hi RGB values based on the tolerance
		gdouble rHi = red/255.0 + tol;
		gdouble gHi = green/255.0 + tol;
		gdouble bHi = blue/255.0 + tol;
		gdouble rLow = red/255.0 - tol;
		gdouble gLow = green/255.0 - tol;
		gdouble bLow = blue/255.0 - tol;

		//correction if the tolerance puts the slider above or below the bounds
		if (rHi > 1) rHi = 1;
		if (gHi > 1) gHi = 1;
		if (bHi > 1) bHi = 1;
		if (rLow < 0) rLow = 0;
		if (gLow < 0) gLow = 0;
		if (bLow < 0) bLow = 0;

		//show the colormap widget and get the int of the tab currently open
		gtk_widget_show(gd->colormap.win);
		ColorRangeWidget *crange;
		crange = COLOR_RANGE_WIDGET(gd->colormap.colorrange);
		gint tabnum = color_range_get_current_tab(crange);

		//change the title of the colormap.win to the RGB and (X,Y)
		char *title;
		asprintf(&title,"aNXTCam - RGB [%i,%i,%i] at X,Y (%i,%i)",red,green,blue,x,y);
		gtk_window_set_title(GTK_WINDOW(gd->colormap.win),title);
		free(title);

		//set the low and high colors of the slider in the color range
		color_range_widget_set_color_low(crange,tabnum,rLow,gLow,bLow);
		color_range_widget_set_color_high(crange,tabnum,rHi,gHi,bHi);
		return TRUE;
	}
	else return FALSE;
}


/**
 * Initializes GUI
 *  @param argc Reference for argc
 *  @param argv Reference for argv
 *  @return GUI descriptor
 *          NULL on failure
 */
gui_gd_t *gui_init(int *argc,char ***argv) {
  char buf[32];
  int i;
  GtkBuilder *builder;
  GError *error = NULL;
  gui_gd_t *gd;

  // init Gtk
  gtk_init(argc,argv);
  g_thread_init(NULL);
  gd = g_slice_new(gui_gd_t);

  // Load XML GUI description
  builder = gtk_builder_new();
  if (!gtk_builder_add_from_file(builder,"gui.glade",&error)) {
    gui_gerror(gd,error);
    return NULL;
  }

  // Load widgets for main window
  gd->main.win = GUI_GET_WIDGET(builder,"win_main");
  gd->main.log.text_log = GUI_GET_WIDGET(builder,"text_log");
  gd->main.log.buf_log = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gd->main.log.text_log));
  gd->main.statusbar.statusbar = GUI_GET_WIDGET(builder,"statusbar");
  gd->main.statusbar.contextid = gtk_statusbar_get_context_id(GTK_STATUSBAR(gd->main.statusbar.statusbar),"statusbar");
  gd->main.statusbar.img_led = GUI_GET_WIDGET(builder,"img_led");

  // Load widgets for colormap window
  gd->colormap.win = GUI_GET_WIDGET(builder,"win_colormap");
  gd->colormap.tolerance = GUI_GET_WIDGET(builder,"spinbtn_colormap"); //widget created for tolerance spin button
  gd->colormap.alertOverlap = GUI_GET_WIDGET(builder,"alertOverlap"); // widget to alert of overlapping colors
  gd->colormap.box_colorrange = GUI_GET_WIDGET(builder,"box_colorrange");
  gd->colormap.colorrange = color_range_widget_new();
  gtk_box_pack_start(GTK_BOX(gd->colormap.box_colorrange),gd->colormap.colorrange,1,1,0);
  g_signal_connect(G_OBJECT(gd->colormap.colorrange),"upload",G_CALLBACK(gui_colormap_upload),gd);
  gtk_widget_show(gd->colormap.colorrange);
  gd->colormap.current = NULL;
  gui_colormap_autoload(gd);

  // Load widgets for frame window
  gd->frame.win = GUI_GET_WIDGET(builder,"win_frame");
  gd->frame.img_frame = GUI_GET_WIDGET(builder,"img_frame");
  gd->frame.chkbtn_interpolate = GUI_GET_WIDGET(builder,"chkbtn_interpolate");
  gd->frame.aspect_frame = GUI_GET_WIDGET(builder,"aspect_frame");
  g_signal_connect (G_OBJECT(gd->frame.win), "button-press-event", G_CALLBACK(button_press_event_cb), gd); //executes the button press function
  gtk_widget_set_events(G_OBJECT(gd->frame.win), gtk_widget_get_events (G_OBJECT(gd->frame.win))|GDK_BUTTON_PRESS_MASK);//button mask for the button press


  // Load widgets for tracking window
  gd->tracking.win = GUI_GET_WIDGET(builder,"win_tracking");
  gd->tracking.draw_objects = GUI_GET_WIDGET(builder,"draw_objects");
  gd->tracking.radio_mode_obj = GUI_GET_WIDGET(builder,"radio_mode_obj");
  gd->tracking.radio_mode_line = GUI_GET_WIDGET(builder,"radio_mode_line");
  gd->tracking.scale_threshold = GUI_GET_WIDGET(builder,"scale_threshold");
  gd->tracking.chkbtn_filter = GUI_GET_WIDGET(builder,"chkbtn_filter");
  gd->tracking.gc_obj = NULL;

  // Load widgets for settings window
  gd->preferences.win = GUI_GET_WIDGET(builder,"win_preferences");
  gd->preferences.chkbtn_whitebalance = GUI_GET_WIDGET(builder,"chkbtn_whitebalance");
  gd->preferences.chkbtn_adjustmode = GUI_GET_WIDGET(builder,"chkbtn_adjustmode");
  gd->preferences.chkbtn_lightfilter = GUI_GET_WIDGET(builder,"chkbtn_lightfilter");
  gd->preferences.btn_device = GUI_GET_WIDGET(builder,"file_device");
  gui_preferences_init(gd);

  // Load widgets for version window
  gd->version.win = GUI_GET_WIDGET(builder,"win_version");
  gd->version.text = GUI_GET_WIDGET(builder,"text_version");

  // Load widgets for ping window
  gd->ping.win = GUI_GET_WIDGET(builder,"win_ping");
  gd->ping.text = GUI_GET_WIDGET(builder,"text_ping");

  // Load widgets for about dialog
  gd->about.dialog = GUI_GET_WIDGET(builder,"dialog_about");

  // connect all signals
  gtk_builder_connect_signals(builder,gd);

  // destroy GtkBuilder
  g_object_unref(G_OBJECT(builder));

  // connect logging with GUI
  gui_log_set_gd(gd);

  logmsg("Running on %s\n",NXTCAM_OS_NAME);

  return gd;
}

/**
 * Runs GUI
 */
void gui_main(gui_gd_t *gd) {
  // show main window
  gtk_widget_show(gd->main.win);

  // once a while sync gui with device
  gd->sync = 1;
  g_timeout_add(50,gui_sync,gd);

  // Run Gtk
  gtk_main();
}


/*
 * File filters
 */

static void gui_file_filter_add_pixbuf(GtkFileChooser *filechooser,GdkPixbufFormat *format) {
  GtkFileFilter *filter;
  int i;
  char wildcard[16];
  char **extensions;

  filter = gtk_file_filter_new();

  gtk_file_filter_set_name(filter,gdk_pixbuf_format_get_name(format));

  extensions = gdk_pixbuf_format_get_extensions(format);
  for (i=0;extensions[i];i++) {
    snprintf(wildcard,16,"*.%s",extensions[i]);
    gtk_file_filter_add_pattern(filter,wildcard);
  }

  gtk_file_chooser_add_filter(filechooser,filter);
}

static void gui_file_filter_add_pixbuf_writable_foreach(void *elem,void *data) {
  GdkPixbufFormat *format;
  GtkFileChooser *filechooser;

  format = (GdkPixbufFormat*)elem;
  filechooser = (GtkFileChooser*)data;

  if (gdk_pixbuf_format_is_writable(format)) {
    gui_file_filter_add_pixbuf(filechooser,format);
  }
}

void gui_file_filter_add_writable_pixbufs(GtkFileChooser *filechooser) {
  GSList *formats;

  formats = gdk_pixbuf_get_formats();
  g_slist_foreach(formats,gui_file_filter_add_pixbuf_writable_foreach,filechooser);
  g_slist_free(formats);
}

void gui_file_filter_add_objtrack(GtkFileChooser *filechooser) {
  GtkFileFilter *filter;

  filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter,"Object tracks");
  gtk_file_filter_add_pattern(filter,"*.obt");

  gtk_file_chooser_add_filter(filechooser,filter);
}

void gui_file_filter_add_colormap(GtkFileChooser *filechooser) {
  GtkFileFilter *filter;

  filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter,"Colormap");
  gtk_file_filter_add_pattern(filter,"*.clm");

  gtk_file_chooser_add_filter(filechooser,filter);
}
