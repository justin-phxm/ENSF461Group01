.PHONY: all
all: memsym.out

memsym.out: memsym.c
	gcc -g -Wall -o $@ $^ -lm

clean:
	rm -f memsym.out
