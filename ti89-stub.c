/* Inspired by the file `m68k-stub.c' shipped with gdb, which says it
 * is in the public domain.
 */

#include <intr.h>
#include <string.h>
#include <stdint.h>

INT_HANDLER traps[255];

extern INT_HANDLER catch_BUS;
extern INT_HANDLER catch_ADDRESS;
extern INT_HANDLER catch_group0;
extern INT_HANDLER catch_group12;
extern INT_HANDLER catch_exception;

struct registers regs;

/* Set up the debugger
 */
void set_debug_traps(void)
{
	debug_stack = malloc(1024 * sizeof(void *));
}

/* Return the system to normal
 */
void remove_debug_traps(void)
{
	free(debug_stack);
}


void *debug_stack;
void *super_stack;
void *user_stack;

int16_t fd_function;
void *fb_fault_addr;
int16_t fb_fault_insn;
int16_t fb_status;
int32_t fb_pc;
int16_t fb_vec_no;

asm("
.text
catch_BUS:
	move.w	#0x08,frame_buf+14
	bra	catch_group0
");

asm("
.text
catch_ADDRESS:
	move.w	#0x0C,frame_buf+14
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
	/* bookkeeping is most of what a debugger does */
	move.w	(%a7)+,fb_function
	move.l	(%a7)+,fb_fault_addr
	move.w	(%a7)+,fb_fault_insn
	/* save registers after popping the above in order to better
	 * reflect the state of the program
	 */
	movem.l	%d0-%d7/%a0-%a7,regs	/* save registers */

	move.l	%a7,super_stack		/* choose the stack */
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
 * vector number & format code
 * (25 extra words if format code = 8)
 */
asm("
.text
catch_group12:
	move.w	(%a7)+,fb_status	/* status register */
	move.l	(%a7)+,fb_pc		/* program counter */
	move.w	(%a7)+,fb_vec_no	/* stack format code in top nibble */
	/* save registers after popping the above in order to better
	 * reflect the state of the program
	 */
	movem.l	%d0-%d7/%a0-%a7,regs

	move.l	%a7,super_stack
	move.l	debug_stack,%a7

	bra	catch_exception
");


/* Exception handler
 */

asm("
.text
catch_exception:
	move.w	fb_status,%d0
	andi.w	#0x2000,%d0
	bne	super_mode
	move.l	%usp,regs+15*4
super_mode:
	move.w	fb_vec_no,-(%sp)
	jbsr	halted

	move.l	%a7,debug_stack
	move.l	super_stack,%a7

	/* Write an exception stack frame.  This works only if the
	 * exception was group 1 or 2.  I'm just going to cross my
	 * fingers and hope.
	 */

	move.w	fb_vec_no,-(%a7)
	move.l	fb_pc,-(%a7)
	move.w	fb_status,-(%a7)

	movem.l	regs,%d0-%d7/%a0-%a6

	rte
");

/* This function executes when the machine is halted; it is the entry
 * point to the debugger proper.
 */
void halted(void)
{
	
}

/* Block until a byte is received from the host gdb.
 */
char recv_byte(void)
{
	return peekIO(0x60000f);
}

char * recv_packet(void)

/* Send a byte to the host gdb and then return.
 */
void send_byte(char c)
{
	pokeIO(0x60000f, c);
	return;
}
