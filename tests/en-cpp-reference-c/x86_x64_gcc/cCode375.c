/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswblank(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 12288;
    printf("In the default locale, iswblank(%#x) = %d\n", c, !!iswblank(c));
    setlocale(6, "en_US.utf8");
    printf("In Unicode locale, iswblank(%#x) = %d\n", c, !!iswblank(c));
}


