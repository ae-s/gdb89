/* Inspired by the file `m68k-stub.c' shipped with gdb, which says it
 * is in the public domain.
 */

#include <tigcclib.h>
#include "ti89-stub.h"

INT_HANDLER *traps;

extern INT_HANDLER catch_group0;
extern INT_HANDLER catch_exception;

volatile struct registers regs;

void halted(uint16_t);

void *debug_stack_seg;
volatile void *debug_stack;
volatile void *super_stack;

volatile void *fb_fault_addr;
volatile int16_t fb_fault_insn;
volatile int16_t fb_status;
volatile int32_t fb_pc;
volatile int16_t fb_vec_no;

DEFINE_INT_HANDLER_ASM(catch_BUS, "
	move.w	#0x08,fb_vec_no
	bra	catch_group0
");

DEFINE_INT_HANDLER_ASM(catch_ADDRESS, "
	move.w	#0x0C,fb_vec_no
	bra	catch_group0
");

/* Group 0 exception handler
 * - Reset
 * - Bus Error
 * - Address Error
 *
 *
 * Stack layout on entry:
 *
 * access adr high     <-- top of supervisor stack
 * access adr low
 * instruction (?)
 * status register
 * pc high
 * pc low
 */
asm("
.text
catch_group0:
	movem.l	%d0-%d7/%a0-%a7,regs	/* save registers */

	move.l	debug_stack,%a7

	bra	catch_exception
");

/* Groups 1 & 2 exception handler
 * Group 1:
 * - Trace
 * - Interrupts
 * - Illegal Instruction
 * - Privilege Violation
 *
 * Group 2:
 * - TRAP
 * - TRAPV
 * - CHK
 * - Divide by 0
 *
 *
 * Stack layout on entry:
 *
 * status register    <-- top of supervisor stack
 * pc high
 * pc low
 */
DEFINE_INT_HANDLER_ASM(catch_group12, "
	move.w	(%a7),fb_status		/* status register */
	move.l	2(%a7),fb_pc		/* program counter */
	movem.l	%d0-%d7/%a0-%a7,regs

	move.l	debug_stack,%a7

	bra	catch_exception
");


/* Exception handler, debugger entry point
 *
 * Once the machine is interrupted, it comes here to enter the
 * debugger.
 */
asm("
.text
catch_exception:
	move.w	fb_status,%d0
	andi.w	#0x2000,%d0
	bne	super_mode

	/* branch not taken: was in user mode */
	move.l	%usp,%a6
	move.l	%a6,regs+(15*4)
super_mode:
	/* Enter the debugger */
	move.w	fb_vec_no,-(%sp)
	bsr	halted

	addq.l	#2,%sp

	lea.l	(%sp),%a0
	move.l	debug_stack,%a1
	cmpa.l	%a0,%a1
	beq	is_ok

	move.l	%a0,-(%sp)
	move.l	%a1,-(%sp)
	bsr	stack_mismatch
	add.l	#8,%sp

is_ok:
	/* if everything is good, the stack points back where it started. */
	move.l	%a7,debug_stack

	movem.l	regs,%d0-%d7/%a0-%a7

	rte
");

/* This function executes when the machine is halted; it is the entry
 * point to the debugger proper.  Everything up to this point is just
 * framing.
 *
 * catch_exception calls halted() once the machine is in a stable
 * state where debugger C code won't disrupt the interrupted code.
 */
void halted(uint16_t vec_no)
{
	printf("Broke in to %x\n", vec_no);
	printf("savesp = %lx\n", regs.a7);
	printf("savepc = %lx\n", fb_pc);
	return;
}

void stack_mismatch(void *isnow, void *shouldbe)
{
	printf("STACK MISMATCH\n"
	       "I see: %lx\n"
	       "Should %lx\n", isnow, shouldbe);
}

/* Set up the debugger
 */
void set_debug_traps(void)
{
	debug_stack_seg = malloc(1024 * sizeof(void *));
	// smack in the middle, because I'm stupid or something.
	debug_stack = debug_stack_seg + (512 * sizeof(void *));
	traps = malloc(255 * sizeof(void *));

	/* TODO:
	 *
	 * trap AI4 while the debugger is in effect, to allow serial
	 * communications to happen.
	 */

	puts("trap 14\n");
	SetIntVec(TRAP_14, catch_group12);
	puts("on\n");
	SetIntVec(INT_VEC_ON_KEY_PRESS, catch_group12);
	puts("int2\n");
	SetIntVec(AUTO_INT_2, catch_group12);
}

/* Return the system to normal
 */
void remove_debug_traps(void)
{
	free(debug_stack_seg);
	free(traps);
}

/* Block until a byte is received from the host gdb.
 */
char recv_byte(void)
{
	return peekIO(0x60000f);
}

char *recv_packet(void)
{

}

/* Send a byte to the host gdb and then return.
 */
void send_byte(char c)
{
	pokeIO(0x60000f, c);
	return;
}
