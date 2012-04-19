CC=tigcc
CFLAGS=-gstabs

.PHONY: install clean

hello.89z: hello.c ti89-stub.c ti89-stub.h
	$(CC) $(CFLAGS) hello.c ti89-stub.c

clean:
	rm -f hello.89z hello.dbg

# '\x115' is "home", '\x0d' is "return".
install: hello.89z
	tiput -m TI89 -c SilverLink --force hello.89z
	tikey -m TI89 '\x115' 'hello()\x0d'
