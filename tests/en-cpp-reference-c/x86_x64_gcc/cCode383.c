/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswspace(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 8195;
    printf("In the default locale, iswspace(%#x) = %d\n", c, !!iswspace(c));
    setlocale(6, "en_US.utf8");
    printf("In Unicode locale, iswspace(%#x) = %d\n", c, !!iswspace(c));
}


