CC=clang
CFLAGS=-O3
all:
	${CC} blobs.c -o blobs ${CFLAGS}
fork:
	${CC} blobs.c -o blobs ${CFLAGS} -DFORKBG
install:
	cp blobs /usr/local/bin/
clean:
	rm blobs