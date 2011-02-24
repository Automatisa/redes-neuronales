TARGET=perseptron_practico3
CC=gcc
CFLAGS+= -ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast -Wcast-qual -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic -g `pkg-config --cflags glib-2.0` -lm
LDFLAGS=`pkg-config --libs glib-2.0`
SOURCES=$(shell echo *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

# BString requiere flags distintos:
$(BSTRING_OBJECTS):CFLAGS=-ansi -Wall -Werror -pedantic -g

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)  $(BSTRING_OBJECTS) .depend *~
	make -C tests clean

test: $(OBJECTS)
	make -C tests test

memtest: $(OBJECTS)
	make -C tests memtest

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
