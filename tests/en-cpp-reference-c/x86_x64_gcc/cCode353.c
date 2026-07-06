/* Cake x86_x64_gcc */

extern unsigned int btowc(int __c);
extern int printf(char * __format, ...);

void try_widen(unsigned char c)
{
    unsigned int w;

    w = btowc(c);
    if (w != 4294967295)
    {
        printf("The single-byte character %#x widens to %#x\n", c, w);
    }
    else
    {
        printf("The single-byte character %#x failed to widen\n", c);
    }
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    char * loc;

    loc = setlocale(6, "lt_LT.iso88594");
    ;
    printf("In Lithuanian ISO-8859-4 locale:\n");
    try_widen(65);
    try_widen(223);
    try_widen(249);
    setlocale(6, "lt_LT.utf8");
    printf("In Lithuanian UTF-8 locale:\n");
    try_widen(65);
    try_widen(223);
    try_widen(249);
}


