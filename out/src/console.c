struct _iobuf {
    void * _Placeholder;
};


void *GetStdHandle(unsigned long nStdHandle);
int GetConsoleMode(void * hConsoleHandle, unsigned long * lpMode);
int SetConsoleMode(void * hConsoleHandle, unsigned long dwMode);
int SetConsoleOutputCP(unsigned int wCodePageID);

unsigned char  enable_vt_mode(void)
{
    unsigned long mode = 0;
    void * h_out = GetStdHandle(((unsigned long) -11));
    if (h_out != ((void *)(long) -1) && GetConsoleMode(h_out, &mode) != 0 && SetConsoleMode(h_out, mode = 4) != 0 && SetConsoleOutputCP(65001) != 0)
    {
        return 1;
    }
    return 0;
}

int _kbhit(void);

int c_kbhit(void)
{
    return _kbhit();
}

int _getch(void);

int c_getch(void)
{
    return _getch();
}

int puts(char * _Buffer);
int fflush(struct _iobuf * _Stream);
struct _iobuf *__acrt_iob_func(unsigned int _Ix);

void c_clrscr()
{
    puts("\x1b[2J\x1b[1;1H");
    fflush((__acrt_iob_func(1)));
}

