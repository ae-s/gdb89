

#include <tigcclib.h>
#include "ti89-stub.h"
#include "comms.h"

/* This is a simple buffer for inbound serial communications. */
#define RXTX_BUF_SIZE 32
volatile char rx_buffer[RXTX_BUF_SIZE];
volatile int rxbuf_len = 0;

/* outbound buffer */
volatile char tx_buffer[RXTX_BUF_SIZE];
volatile int txbuf_len = 0;

enum rxtx_s { s_idle, s_rx, s_tx, s_error } rxtx_state;

/* Block until a byte is received from the host gdb.
 */
char recv_byte(void)
{
	int p = -1;
	while (p == -1)
		p = fetch_byte(&rx_buffer, &rxbuf_len);

	return p;
}

char *recv_packet(void)
{

}

/* Auto-Int 4 handler for serial communication
 */
DEFINE_INT_HANDLER(catch_ai4)
{
	unsigned char reason;

	DrawStr(0, 65, "LNK INTR", A_NORMAL);

	reason = peekIO(0x60000d);

	if (reason & 1<<3 == 0) {
		// nothing is happening
		rxtx_state = s_idle;
		DrawStr(0, 65, "Nothing", A_NORMAL);
		return;
	}

	// error, reset
	if (reason & 1<<7 == 0) {
		DrawStr(0, 65, "Error", A_NORMAL);
		rxtx_state = s_error;
		pokeIO(0x60000c, 0xe0);
		pokeIO(0x60000c, 0x8d);
		rxtx_state = s_idle;
	}

	// byte is waiting to be read
	if (reason & 1<<5 == 0) {
		DrawStr(0, 65, "Reading", A_NORMAL);
		int b = peekIO(0x60000f);
		stuff_byte(b, &rx_buffer, &rxbuf_len);
		rxtx_state = s_tx;
	}

	if (reason & 1<<2 == 0)
		;

	// ready to write, queue empty
	if (reason & 1<<6 != 0) {
		DrawStr(0, 65, "Queue empty", A_NORMAL);
		int c;
		rxtx_state = s_idle;

		c = fetch_byte(&tx_buffer, &txbuf_len);
		if (c == -1)
			return;

		pokeIO(0x60000f, (unsigned char)c);
	}
}

/* Stuff a byte into the given buffer.  Returns 0 on success, -1 on
 * failure.
 */
int stuff_byte(char c, char *buf, int *end)
{
	/* first, check for buffer full */
	if (*end == RXTX_BUF_SIZE)
		return -1;

	buf[(*end)++] = c;

	return 0;
}

/* Pull a byte off the given circular buffer */
int fetch_byte(char *buf, int *end)
{
	char c;
	/* check for buffer empty */
	if (*end == 0)
		return -1;

	c = buf[0];

	memmove(buf, buf+1, (*end) - 1);

	return c;
}

/* Send a byte to the host gdb and then return.
 */
void send_byte(char c)
{
	pokeIO(0x60000f, c);
	return;
}
