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
	// Get $USER
	char* user = getenv("USER");
	const char* msg = "This workstation is locked\n";

	// Ignore requests to exit
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	// Set up ncurses screen
	initscr();
	raw();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	// Move to center and print info for the user
	move(row/2, (col-(strlen(msg)))/2);
	printw(msg);

	// Start pam conversation
	ret = pam_start("check_user", getenv("USER"), &pconv, &pamh);
	
	// Check if conversation was successful (user has authenticated)
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
		// Warn the user if they haven't been authenticated
		// There should be a loop here FIXME
		printw("Not authenticated\n");
		getch();
	}

	if (pam_end(pamh,ret) != PAM_SUCCESS){
		pamh = NULL;
		endwin();
		exit(1);
	}
	// Close ncurses screen and window
	endwin();
	return ( ret = PAM_SUCCESS ? 0:1);
}
