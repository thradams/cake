/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswlower(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 1092;
    printf("In the default locale, iswlower(%#x) = %d\n", c, !!iswlower(c));
    setlocale(6, "en_US.utf8");
    printf("In Unicode locale, iswlower(%#x) = %d\n", c, !!iswlower(c));
}


