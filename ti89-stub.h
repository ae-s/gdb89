#ifndef TI89_STUB_H
#define TI89_STUB_H

#include <stdint.h>

struct registers {
	int32_t d0, d1, d2, d3, d4, d5, d6, d7;
	int32_t a0, a1, a2, a3, a4, a5, a6, a7;
};

void set_debug_traps(void);
void remove_debug_traps(void);

void send_byte(char);
char recv_byte(void);

#define breakpoint() asm("trap	#14")

/* Much like DEFINE_INT_HANDLER, but allows you to use specific asm
 * for the entire interrupt handler body, thus preserving the machine
 * state by default.
 */
#define DEFINE_INT_HANDLER_ASM(name, func) extern _DEREF_INT_HANDLER name[]; asm(".xdef __ref_all___custom_int_handlers;"); extern void __##name##_body__ (void) asm( #name ); asm(#name ": \
" func);

#endif
