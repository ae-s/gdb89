#define USE_TI89

#include <tigcclib.h>

void _main(void);
void putDebugChar(int);
int getDebugChar(void);
void exceptionHandler(int, void *);

/* Where GDB goes after a bus error, etc. */
void (*exceptionHook)() = NULL;

void _main(void)
{
	while (1) {
		puts("Hello, World!\n");
	}
}

// Used to talk to the remote debugger
void putDebugChar(int ch)
{
	char c = ch;
	OSWriteLinkBlock(&c, 1);
}

// Used to listen to the remote debugger
int getDebugChar(void)
{
	char c;
	int len;
	len = OSReadLinkBlock(&c, 1);

	if (len != 0)
		return c;
	else
		return 0;
}

/* Used by the GDB stub to set a vector handler */
void exceptionHandler(int vec_no, void *func)
{
	SetIntVec((long) vec_no, (INT_HANDLER) func);
}
