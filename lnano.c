/*
 * Author: Ivan Romeo
 * Date start project: 2024-11-05
 */

//INCLUDES

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
//DEFINITIONS 
#define CTRL_KEY(k) ((k) & 0x1f)
//DATA
struct editorState {
	int screen_rows;
	int screen_cols;
	struct termios orig_term;
};
struct editorState E;
//OUTPUT
void editorDrawRows(){
	int y;
	for(y = 0; y < E.screen_rows; y++){
		write(STDOUT_FILENO,"~\r\n",3);
	}
}
void editorRefreshScreen(){
        write(STDOUT_FILENO,"\x1b[2J",4);
        write(STDOUT_FILENO,"\x1b[H",3);
}
void editorSetScreen(){
	editorRefreshScreen();
	editorDrawRows();
	write(STDOUT_FILENO,"\x1b[H",3);
}
//TERMINAL

int getWindowSize(int *rows, int *cols) {
	struct winsize ws;
	if(1||ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
		if(write(STDOUT_FILENO,"\x1b[999C\x1b[999B",12) != 12) return -1;
		editorReadKey();
		return -1;
	} else{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}

}
void die(const char *s){
	editorRefreshScreen();
	perror(s);
	exit(1);
}
void disableRawMode(){
	if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &E.orig_term) == -1)
		die("tcsetattr");
}
void enableRawMode() {
	if(tcgetattr(STDIN_FILENO,&E.orig_term) == -1)
		die("tcgetattr");
	atexit(disableRawMode);

	struct termios raw = E.orig_term;
	raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);
	raw.c_iflag &= ~(IXON|ICRNL|BRKINT|INPCK|ISTRIP);
	raw.c_cflag |= (CS8);
	raw.c_oflag &= ~(OPOST);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}
char editorReadKey() {
	int nread;
	char c;
	while((nread = read(STDIN_FILENO, &c, 1)) != 1){
		if(nread == -1 && errno != EAGAIN) die("read");
	}
	return c;
}
void editorProcessKeypress(){
	char c = editorReadKey();
	switch (c){
		case CTRL_KEY('q'):
			editorRefreshScreen();
			exit(0);
			break;
	}
}



//INIT
void initEditor() {
	if(getWindowSize(&E.screen_rows, &E.screen_cols) == -1) die("getWindowSize");
}
int main(){
	enableRawMode();
	initEditor();
	while (1){
		editorSetScreen();
		editorProcessKeypress();
	}
	return 0;
}
