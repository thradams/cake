
#ifdef _WIN32
#include <Windows.h>
#else
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include "console.h"

#ifndef WIN32

bool EnableVTMode(void)
{
    return true;
}

int c_kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

/* Read 1 character without echo */
int c_getch(void)
{
    struct termios old, new;
    int ch;

    tcgetattr(0, &old);

    new = old;
    new.c_lflag &= ~ICANON;
    new.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &new);

    ch = getchar();

    tcsetattr(0, TCSANOW, &old);

    return ch;
}

#else

bool EnableVTMode(void)
{
    DWORD dwMode = 0;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if ((hOut = GetStdHandle(STD_OUTPUT_HANDLE)) != INVALID_HANDLE_VALUE &&
            GetConsoleMode(hOut, &dwMode) != 0 &&
            SetConsoleMode(hOut, dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0 &&
            SetConsoleOutputCP(CP_UTF8) != 0)
    {
        return true;//ok
    }
    return false;//error
}

int c_kbhit(void)
{
    return _kbhit();
}
int c_getch(void)
{
    return _getch();
}

#endif

void c_clrscr()
{
    puts("\x1b[2J\x1b[1;1H");
    fflush(stdout);
}
