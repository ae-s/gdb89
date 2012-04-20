#define USE_TI89

#include <tigcclib.h>
#include "ti89-stub.c"

void _main(void);

short sr;
void _main(void)
{
	long int x;

	clrscr();
	set_debug_traps();
	printf_xy(0, 60, "_main = %lp\n", &_main);
//	for (x = 0 ; x++ ; x < 1000000) {}
//	breakpoint();
//	puts(".\n");

	while (1) {};
}

