/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern unsigned int short **__ctype_b_loc(void);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    unsigned char c;

    c = 198;
    printf("In the default C locale, \\xc6 is %suppercase\n", ((*__ctype_b_loc())[(int)(c)] & 256) ? "" : "not ");
    setlocale(6, "en_GB.iso88591");
    printf("In ISO-8859-1 locale, \\xc6 is %suppercase\n", ((*__ctype_b_loc())[(int)(c)] & 256) ? "" : "not ");
}


