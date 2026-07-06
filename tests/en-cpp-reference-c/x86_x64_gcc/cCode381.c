/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswprint(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;
    int c2;

    c = 8194;
    printf("in the default locale, iswprint(%#x) = %d\n", c, !!iswprint(c));
    setlocale(6, "en_US.utf8");
    printf("in Unicode locale, iswprint(%#x) = %d\n", c, !!iswprint(c));
    c2 = 130;
    printf("in Unicode locale, iswprint(%#x) = %d\n", c2, !!iswprint(c2));
}


