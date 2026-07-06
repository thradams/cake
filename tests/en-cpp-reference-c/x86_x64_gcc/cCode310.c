/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern unsigned int short **__ctype_b_loc(void);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    unsigned char c;

    c = 223;
    printf("isalnum('\\xdf') in default C locale returned %d\n", !!((*__ctype_b_loc())[(int)(c)] & 8));
    if (setlocale(0, "de_DE.iso88591"))
    {
        printf("isalnum('\\xdf') in ISO-8859-1 locale returned %d\n", !!((*__ctype_b_loc())[(int)(c)] & 8));
    }
}


