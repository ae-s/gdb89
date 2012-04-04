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

void breakpoint(void);

#endif
