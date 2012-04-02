CC=tigcc
CFLAGS=-gstabs

.PHONY: install clean

hello.89z: hello.c m68k-stub.c
	$(CC) $(CFLAGS) hello.c m68k-stub.c

clean:
	rm -f hello.89z

install: hello.89z
	tilp --calc=ti89 --cable=SilverLink --no-gui hello.89z

