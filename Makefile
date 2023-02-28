CFLAGS=-std=c11 -g -static
rcc: rcc.c

test: rcc
		./test.sh

clean:
		rm -f rcc *.o *~ tmp+

.PHONY: test clean