/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswalnum(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 5037;
    printf("in the default locale, iswalnum(%#x) = %d\n", c, !!iswalnum(c));
    setlocale(6, "en_US.utf8");
    printf("in Unicode locale, iswalnum(%#x) = %d\n", c, !!iswalnum(c));
}


