.PHONY: all
all: prefixscan.out

prefixscan.out: prefixscan.c
	gcc -g -Wall -o $@ $^ -pthread -lm

clean:
	rm -f prefixscan.out
