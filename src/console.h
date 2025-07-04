/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

#include <stdbool.h>

bool enable_vt_mode(void);

/*change foreground color*/

#define BLACK     "\x1b[30m" 
#define BLUE     "\x1b[34m"
#define GREEN     "\x1b[32m"
#define CYAN     "\x1b[36m"
#define RED "\x1b[31;1m"
#define MAGENTA     "\x1b[35m"
#define BROWN     "\x1b[31m"
#define LIGHTGRAY "\x1b[37m"
#define DARKGRAY "\x1b[90m"
#define LIGHTBLUE    "\x1b[34;1m"
#define  LIGHTGREEN "\x1b[92m"
#define LIGHTCYAN "\x1b[36;1m"
#define LIGHTRED "\x1b[91m"
#define LIGHTMAGENTA "\x1b[95m"
#define YELLOW "\x1b[93m"
#define WHITE "\x1b[97m"

//https//en.wikipedia.org/wiki/ANSI_escape_code


#define BK_BLACK "\x1b[40m"
#define BK_BLUE "\x1b[44m"
#define BK_GREEN  "\x1b[42m"
#define BK_CYAN "\x1b[46m"
#define BK_RED "\x1b[41;1m"
#define BK_MAGENTA "\x1b[45m"
#define BK_BROWN "\x1b[41m"
#define BK_LIGHTGRAY "\x1b[40;1m"
#define BK_DARKGRAY "\x1b[40m"
#define BK_LIGHTBLUE "\x1b[44;1m"
#define BK_LIGHTGREEN "\x1b[42,1m"
#define BK_LIGHTCYAN "\x1b[46;1m"
#define BK_LIGHTRED "\x1b[41;1m"
#define BK_LIGHTMAGENTA "\x1b[45;1m"
#define BK_YELLOW             "\x1b[43;1m"
#define BK_WHITE             "\x1b[47;1m"
#define BK_BLINK "\x1b[40m"

#define RESET "\x1b[0m"


int c_kbhit(void);
int c_getch(void);
void c_clrscr();
void c_gotoxy(int x, int y);