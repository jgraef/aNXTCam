CFLAGS = -Iinclude/ \
         `pkg-config --cflags gtk+-2.0` \
         -D_GNU_SOURCE=1
DEPENDFILE = .depend

.PHONY: all
.PHONY: dep
.PHONY: clean

LIBS = libcolorrange.a libcolorrange_widget.a libcolorpreview.a

all: dep $(LIBS)

clean:
	rm -f *.o $(LIBS)

libcolorrange.a: colorrange.o
	$(AR) rs $@ $^

libcolorrange_widget.a: colorrange_widget.o
	$(AR) rs $@ $^

libcolorpreview.a: colorpreview.o
	$(AR) rs $@ $^

-include $(DEPENDFILE)
