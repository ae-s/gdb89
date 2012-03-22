CC=tigcc
CFLAGS=-gstabs

hello.89z: hello.c m68k-stub.c
	$(CC) $(CFLAGS) hello.c m68k-stub.c
