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
	long int x;
	puts("foo\n");
	puts("Setting traps\n");
	set_debug_traps();
	for (x = 0 ; x++ ; x < 1000000) {}
	puts("breakpointing\n");
	breakpoint();

	while (1) {
//		puts("Hello, World!\n");
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
	void *old, *new;
	old = GetIntVec((long) vec_no * 4);
	printf("Setting %xd (%lp) ...\n", vec_no * 4, func);

	if (vec_no * 4 > TRAP_15) {
		printf(" - Ignoring, >#15\n");
		return;
	}

	if ((vec_no * 4 > FIRST_AUTO_INT) && (vec_no * 4 < LAST_AUTO_INT)) {
		printf(" - Ignoring AI\n");
	}

	SetIntVec((long) vec_no * 4, (INT_HANDLER) func);
	new = GetIntVec((long) vec_no * 2);
	printf(" - %lp -> %lp\n", old, new);
}
