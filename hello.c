#define USE_TI89

#include <tigcclib.h>
#include "ti89-stub.c"

void _main(void);

void _main(void)
{
	long int x;
	puts("foo\n");
	puts("Setting traps\n");
	set_debug_traps();
//	halted(0);
//	puts("break\n");
	printf("catcher is: %lp\n", catch_group12);
	printf("Trap 14 is: %lp\n", GetIntVec(TRAP_14));
	for (x = 0 ; x++ ; x < 100000) {}
	breakpoint();
	puts(".\n");

	while (1) {}
}

