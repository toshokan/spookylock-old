#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>

#include "auth.h"

#define USER_BUF_SIZE 33

int main(){
	static struct pam_conv pconv;
	pconv.conv = &ncurses_conv;
	pconv.appdata_ptr = NULL;
	pam_handle_t* pamh = NULL;
	int ret;
	int row, col;
	char* user = getenv("USER");
	const char* msg = "This workstation is locked\n";

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	initscr();
	//cbreak();
	raw();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	move(row/2, (col-(strlen(msg)))/2);
	printw(msg);

	ret = pam_start("check_user", getenv("USER"), &pconv, &pamh);
	
	if(ret == PAM_SUCCESS){
		ret = pam_authenticate(pamh, 0);
	}
	if(ret == PAM_SUCCESS){
		ret = pam_acct_mgmt(pamh, 0);
	}
	if(ret == PAM_SUCCESS){
		//printw("Authenticated\n");
		endwin();
		exit(0);
	}else{
		printw("Not authenticated\n");
		getch();
	}

	if (pam_end(pamh,ret) != PAM_SUCCESS){
		pamh = NULL;
		endwin();
		exit(1);
	}
	//getch();
	endwin();
	return ( ret = PAM_SUCCESS ? 0:1);
}
