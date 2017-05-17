#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>

#include "auth.h"

#define USER_BUF_SIZE 33
#define FLAG_NOT_AUTH 0
#define FLAG_AUTH 1

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
	const char* errormsg = "Error: Not authenticated!\n";
	int flag;
	int attempts;

	// Ignore requests to exit
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	// Set up ncurses screen
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	
	flag = 0;
	attempts = 0;
	// Loop until the user is authenticated
	while(flag == FLAG_NOT_AUTH){
		// Get screen dimensions
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
			// Flag as authenticated, do not loop (future)
			flag = FLAG_AUTH;
			endwin();
			exit(0);
		}else{
			// Warn the user if they haven't been authenticated
			mvprintw(row-1,col-(strlen(errormsg)),errormsg);
			refresh();
			sleep(1);
			clear();
			// Reset flag (future)
			flag = FLAG_NOT_AUTH;
			attempts++;
		}
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
