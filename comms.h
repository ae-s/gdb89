#ifndef COMMS_H
#define COMMS_H

char recv_byte(void);
char *recv_packet(void);
int stuff_byte(char, char*, int*);
int fetch_byte(char*, int*);
void send_byte(char);

#endif
