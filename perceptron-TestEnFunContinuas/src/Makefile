TARGET=perceptron-TestEnFunContinuas
CC=gcc
CFLAGS+= -ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast -Wcast-qual -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic -lm -pthread# -g `pkg-config --cflags glib-2.0`
#LDFLAGS=`pkg-config --libs glib-2.0`
SOURCES_EXTERNC=$(shell echo neural-networks/*/*.c)
SOURCES_LOCALC=$(shell echo *.c)
OBJECTS_LOCALC=$(SOURCES_LOCALC:.c=.o)
OBJECTS_EXTERNC=$(SOURCES_EXTERNC:.c=.o)

all: $(TARGET)

# BString requiere flags distintos:
$(BSTRING_OBJECTS):CFLAGS=-ansi -Wall -Werror -pedantic -g

$(TARGET): $(OBJECTS_EXTERNC) $(OBJECTS_LOCALC)
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS_EXTERNC) $(OBJECTS_LOCALC)  $(BSTRING_OBJECTS) .depend *~
	make -C tests clean

test: $(OBJECTS_EXTERNC) $(OBJECTS_LOCALC)
	make -C tests test

memtest: $(OBJECTS_EXTERNC) $(OBJECTS_LOCALC)
	make -C tests memtest

.depend: *.[ch]
	$(CC) -MM $(SOURCES_LOCALC) $(SOURCES_EXTERNC) >.depend

-include .depend

.PHONY: clean all

