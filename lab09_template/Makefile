.PHONY: all

LIBS=$(shell pkg-config --libs MagickWand libdmtx)
CFLAGS=$(shell pkg-config --cflags MagickWand libdmtx) -g -Wall -pthread
 
all: analyzedmtx.out

analyzedmtx.out: analyzedmtx.c 
	gcc $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -f *.o *.out
