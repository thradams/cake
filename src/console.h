#pragma once

#include <stdbool.h>

#ifdef _WIN32
#include <conio.h>
#else
//#include <curses.h>
#endif



bool enable_vt_mode(void);

/*
  DISABLE_COLORS is defined to generate a
  version of cake that does not ouput colors
  A runtime flag msvcouput is already used..
  but some utility functions are not using
*/
#ifdef ENABLE_COLORS
#define COLOR_ESC(x) x
#define COLOR_ESC_PRINT(x) x
#define ESC "\x1b"
#define CSI "\x1b["
#else
#define COLOR_ESC(x) ""
#define COLOR_ESC_PRINT(x)
#define ESC ""
#define CSI ""

#endif

/*change foreground color*/


#define BLACK     COLOR_ESC("\x1b[30m")
#define BLUE     COLOR_ESC("\x1b[34m")
#define GREEN     COLOR_ESC("\x1b[32m")
#define CYAN     COLOR_ESC("\x1b[36m")
#define RED COLOR_ESC("\x1b[31;1m")
#define MAGENTA     COLOR_ESC("\x1b[35m")
#define BROWN     COLOR_ESC("\x1b[31m")
#define LIGHTGRAY COLOR_ESC("\x1b[37m")
#define DARKGRAY COLOR_ESC("\x1b[90m")
#define LIGHTBLUE    COLOR_ESC("\x1b[34;1m")
#define  LIGHTGREEN COLOR_ESC("\x1b[92m")
#define LIGHTCYAN COLOR_ESC("\x1b[36;1m")
#define LIGHTRED COLOR_ESC("\x1b[91m")
#define LIGHTMAGENTA COLOR_ESC("\x1b[95m")
#define YELLOW COLOR_ESC("\x1b[93m")
#define WHITE COLOR_ESC("\x1b[97m")

    //https//en.wikipedia.org/wiki/ANSI_escape_code


#define BK_BLACK COLOR_ESC("\x1b[40m")
#define BK_BLUE COLOR_ESC("\x1b[44m")
#define BK_GREEN  COLOR_ESC("\x1b[42m")
#define BK_CYAN COLOR_ESC("\x1b[46m")
#define BK_RED COLOR_ESC("\x1b[41;1m")
#define BK_MAGENTA COLOR_ESC("\x1b[45m")
#define BK_BROWN COLOR_ESC("\x1b[41m")
#define BK_LIGHTGRAY COLOR_ESC("\x1b[40;1m")
#define BK_DARKGRAY COLOR_ESC("\x1b[40m")
#define BK_LIGHTBLUE COLOR_ESC("\x1b[44;1m")
#define BK_LIGHTGREEN COLOR_ESC("\x1b[42,1m")
#define BK_LIGHTCYAN COLOR_ESC("\x1b[46;1m")
#define BK_LIGHTRED COLOR_ESC("\x1b[41;1m")
#define BK_LIGHTMAGENTA COLOR_ESC("\x1b[45;1m")
#define BK_YELLOW             COLOR_ESC("\x1b[43;1m")
#define BK_WHITE             COLOR_ESC("\x1b[47;1m")
#define BK_BLINK COLOR_ESC("\x1b[40m")

#define RESET COLOR_ESC(ESC "[0m")


int c_kbhit(void);
int c_getch(void);

void c_clrscr();
