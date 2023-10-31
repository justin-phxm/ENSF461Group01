.PHONY: all
all: memsym.out

memsym.out: memsym.c
	gcc -g -Wall -o $@ $^

clean:
	rm -f memsym.out
