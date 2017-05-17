#ifndef AUTH_H
#define AUTH_H
#define IBUF_SIZE 100

#include <security/pam_appl.h>

int iface(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *data);
int ncurses_conv(int n, const struct pam_message **msg,	struct pam_response **resp, void *data);
#endif
