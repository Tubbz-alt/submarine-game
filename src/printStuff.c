#include "units.h"
#include "macros.h"
#include "fireTor.h"
#include "printStuff.h"
#include "setOpts.h"
#include <time.h>

//---------------------------------------- prototypes ----------------------------------------------


//---------------------------------------- code ---------------------------------------------------

void animateTorpedo(const int limit) {
	for(int i = 1; i <= limit; i++) {
		switch (player_sub->direction_facing) {
			case FORWARD: printToMain(player_sub->x,player_sub->y-i,"|");
				break;
			case BACK: printToMain(player_sub->x,player_sub->y+i,"|");
				break;
			case LEFT: printToMain(player_sub->x-(i * X_NORM),player_sub->y,"-");
				break; 
			case RIGHT: printToMain(player_sub->x+(i * X_NORM),player_sub->y,"-");
				break;
			default: //do nothing should never reach this one.
				break;
		}			
		nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);
		printPieces();
	}
}

//updates the display to show ammount of action points you have left
void updateAPDisplay(void) {
	mvwprintw(status_win,4,0,"AP points: %d ",player_sub->ap);
	wrefresh(status_win);
}
//update the display to show how many turns since last detected
void updateLastDetected(void) {
	if(player_sub->detected) {
		PLAYER->last_detected++;
	}
	mvwprintw(status_win,3,0,"detected: %d ",PLAYER->last_detected);
	wrefresh(status_win);
}
//updates the display to show how many AOE torpedoes you have left
void updateAOETorDisplay(void) {
	mvwprintw(status_win,2,0,"AOE Tor: %d",PLAYER->num_aoetor);
	wrefresh(status_win);
}

//update the display to show current health
void updateHealthDisplay(void) {
	mvwprintw(status_win,1,0,"Health: %d  ",player_sub->health);
	wrefresh(status_win);
}

//updates display to show current location as x y z
void updateLocationDisplay(void) {
	mvwprintw(status_win,0,10,"          ");  //blank line to clear line 0,0
	mvwprintw(status_win,0,0,"Locations: %d %d %d",(player_sub->x / X_NORM),player_sub->y,player_sub->z);
	wrefresh(status_win);
}

void printToOptWin(const int x, const int y, const char *const str) {
	mvwprintw(opt_win,y,x,"%s",str);
	wrefresh(opt_win);
}

//print str to main window at x,y
void printToMain(const int x, const int y, const char *const str) {
	mvwprintw(main_win,y,x,"%s",str);
	wrefresh(main_win);
}

//print str to text_win at location x,y
void printToTxtScr(const int x, const int y, const char *const str) {
	wclear(text_win);
	mvwprintw(text_win,y,x,"%s",str);
	wrefresh(text_win);
}

void printLastDetected(SHIP *const ship) {
	if(ship->detected && ship->type == SUBMARINE) {
		wattron(main_win,COLOR_PAIR(BLUE_COLOR));
		printToMain(ship->last_knownx,ship->last_knowny,"O"); //display blue icon where sub is located
		wattroff(main_win,COLOR_PAIR(BLUE_COLOR));
	}
}

void printNumSign(const int i, const int j, const int x, const int y, const int color) {
	wattron(main_win,COLOR_PAIR(color));
	printToMain(x + j,y - i,"#");
	printToMain(x - j,y - i,"#");
	printToMain(x + j,y + i,"#");
	printToMain(x - j,y + i,"#");
	wattroff(main_win,COLOR_PAIR(color));
}

void displaySonar(void) {
	int offset = 6;
	for(int i = 0; i < 7; i++) {
		for(int j = 0; j <= offset * X_NORM; j += X_NORM) {
			printNumSign(i,j,player_sub->x,player_sub->y,GREEN_COLOR);
			nanosleep((const struct timespec[]){{0, 4500000L}}, NULL);
			checkIfDetected(i,j);
		}
		offset--;
	}
	getch();
}

void printOptWin(void) {
	mvwprintw(opt_win,0,1,"[ ]  turbo");
	mvwprintw(opt_win,1,1,"[ ]  AOE tor");
	mvwprintw(opt_win,2,1,"[ ]  sonar");
	mvwprintw(opt_win,3,1,"[ ]  repair");
	wrefresh(opt_win);
}

void printPlayerSub(void) {
	mvwprintw(main_win,player_sub->y,player_sub->x,"x");
	wrefresh(main_win);
}

void printPieces(void) {
	wclear(main_win);
	mvwprintw(main_win,player_sub->y,player_sub->x,"X");
	ENEMIES *head = enemies;
	while(head != NULL) {
		switch(head->ship->type) {
		case SUBMARINE: printLastDetected(head->ship);	
			break;
		case SURFACESHIP: mvwprintw(main_win,head->ship->y,head->ship->x,"z");
			break;
		default: mvwprintw(main_win,head->ship->y,head->ship->x,"c");
			break;			
		}
		head = head->next;
	}
	wrefresh(main_win);
}