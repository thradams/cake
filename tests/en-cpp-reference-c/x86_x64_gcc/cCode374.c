/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswalpha(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 2947;
    printf("in the default locale, iswalpha(%#x) = %d\n", c, !!iswalpha(c));
    setlocale(6, "en_US.utf8");
    printf("in Unicode locale, iswalpha(%#x) = %d\n", c, !!iswalpha(c));
}


