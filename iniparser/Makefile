CFLAGS = -Iinclude/
DEPENDFILE = .depend

SRC = dictionary.c \
      iniparser.c \

OBJ = $(SRC:%.c=%.o)

.PHONY: all
.PHONY: dep
.PHONY: clean

all: dep libiniparser.a

dep: $(SRC)
	$(CC) $(CFLAGS) -MM $(SRC) > $(DEPENDFILE)

libiniparser.a: $(OBJ)
	$(AR) rs $@ $^

clean:
	rm -f libiniparser.a $(OBJ)
	rm -f $(DEPENDFILE)

-include $(DEPENDFILE)
