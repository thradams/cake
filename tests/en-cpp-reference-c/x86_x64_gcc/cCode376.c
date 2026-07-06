/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswcntrl(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 8232;
    printf("In the default locale, iswcntrl(%#x) = %d\n", c, !!iswcntrl(c));
    setlocale(6, "en_US.utf8");
    printf("In Unicode locale, iswcntrl(%#x) = %d\n", c, !!iswcntrl(c));
}


