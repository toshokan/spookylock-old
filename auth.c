#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "auth.h"


int ncurses_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *data){
	struct pam_response* pam_resp;
	char buffer[PAM_MAX_RESP_SIZE];
	char ibuf[IBUF_SIZE];
	int i;
	int row, col;

	getmaxyx(stdscr,row,col);
	//data = data;
	if (num_msg <= 0 || num_msg > PAM_MAX_NUM_MSG)
		return (PAM_CONV_ERR);
	if ((pam_resp = calloc(num_msg, sizeof *pam_resp)) == NULL)
		return (PAM_BUF_ERR);
	
	for (i = 0; i < num_msg; ++i) {
		pam_resp[i].resp_retcode = 0;
		pam_resp[i].resp = NULL;
		
		switch (msg[i]->msg_style) {
			case PAM_PROMPT_ECHO_OFF:
				noecho();
				move((row/2)+1,(col-strlen(msg[i]->msg))/2);
				printw(msg[i]->msg);
				getstr(ibuf);
				pam_resp[i].resp = strdup(ibuf);
				echo();
				if (pam_resp[i].resp == NULL)
					goto failed_conv;
				break;
				memset(ibuf,0,IBUF_SIZE);
			case PAM_PROMPT_ECHO_ON:
				//fputs(msg[i]->msg, stderr);
				printw(msg[i]->msg);
				if (fgets(buffer, sizeof buffer, stdin) == NULL)
					goto failed_conv;
				pam_resp[i].resp = strdup(buffer);
				if (pam_resp[i].resp == NULL)
					goto failed_conv;
				break;
			case PAM_ERROR_MSG:
				fputs(msg[i]->msg, stderr);
				if (strlen(msg[i]->msg) > 0 &&
				    msg[i]->msg[strlen(msg[i]->msg) - 1] != '\n')
					fputc('\n', stderr);
				break;
			case PAM_TEXT_INFO:
				//fputs(msg[i]->msg, stdout);
				printw(msg[i]->msg);
				if (strlen(msg[i]->msg) > 0 &&
				    msg[i]->msg[strlen(msg[i]->msg) - 1] != '\n')
					fputc('\n', stdout);
				break;
			default:
				goto failed_conv;
		}
	}
	*resp = pam_resp;
	return (PAM_SUCCESS);

 failed_conv:
        for (i = 0; i < num_msg; ++i) {
                if (pam_resp[i].resp != NULL) {
                        memset(pam_resp[i].resp, 0, strlen(pam_resp[i].resp));
                        free(pam_resp[i].resp);
                }
        }
        memset(pam_resp, 0, num_msg * sizeof *pam_resp);
	*resp = NULL;
	return (PAM_CONV_ERR);
}