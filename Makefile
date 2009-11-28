CFLAGS = -Iinclude/ \
         -Iwidgets/include \
         -Iiniparser/include \
         `gtk-config --cflags` `pkg-config --cflags gmodule-2.0` \
         -D_GNU_SOURCE=1

LDFLAGS = -Lwidgets/ -lcolorrange_widget -lcolorrange -lcolorpreview \
          -Liniparser/ -liniparser \
          `gtk-config --libs gtk gthread` `pkg-config --libs gmodule-2.0` \
          -lpthread \
          -lm

OSFLAGS = -D__LINUX__

DEPENDFILE = .depend

SRC = colormap.c com.c device.c frame.c main.c \
      gui/about.c gui/colormap.c gui/frame.c gui/log.c gui/main.c gui/msgbox.c \
      gui/preferences.c gui/statusbar.c gui/tracking.c gui/version.c gui/ping.c

.PHONY: all
.PHONY: dep
.PHONY: clean
.PHONY: widgets
.PHONY: iniparser

all: dep widgets iniparser anxtcam

clean:
	rm -f anxtcam
	make -C widgets/ clean
	make -C iniparser/ clean

anxtcam: $(SRC)
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $^ $(LDFLAGS)

widgets:
	make -C widgets/

iniparser:
	make -C iniparser/

-include $(DEPENDFILE)
