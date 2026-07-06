/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswpunct(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 8273;
    printf("in the default locale, iswpunct(%#x) = %d\n", 8273, !!iswpunct(8273));
    setlocale(6, "en_US.utf8");
    printf("in Unicode locale, iswpunct(%#x) = %d\n", 8273, !!iswpunct(8273));
}


