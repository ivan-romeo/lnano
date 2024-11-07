/*
 * Author: Ivan Romeo
 * Date start project: 2024-11-05
 */
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

struct termios orig_term;

void disableRawMode(){
	tcsetattr(STDIN_FILENO,TCSAFLUSH, &orig_term);
}
void enableRawMode() {
	tcgetattr(STDIN_FILENO,&orig_term);
	atexit(disableRawMode);

	struct termios raw = orig_term;
	raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);
	raw.c_iflag &= ~(IXON|ICRNL);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(){
	enableRawMode();

	char c;
	while (read(STDIN_FILENO, &c,1) == 1 && c != 'q'){
		if(iscntrl(c)){
			printf("%d\n",c);
		}else{
			printf("%d ('%c')\n",c,c);
		}
	}
	return 0;
}
