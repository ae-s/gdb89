CC=tigcc
CFLAGS=-gstabs

.PHONY: install clean

hello.89z: hello.c ti89-stub.c ti89-stub.h
	$(CC) $(CFLAGS) hello.c ti89-stub.c

clean:
	rm -f hello.89z hello.dbg

install: hello.89z
	tilp --calc=ti89 --cable=SilverLink --no-gui hello.89z

