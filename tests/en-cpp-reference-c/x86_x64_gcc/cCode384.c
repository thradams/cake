/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswupper(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 1341;
    printf("In the default locale, iswupper(%#x) = %d\n", c, !!iswupper(c));
    setlocale(6, "en_US.utf8");
    printf("In Unicode locale, iswupper(%#x) = %d\n", c, !!iswupper(c));
}


