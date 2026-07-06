/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswgraph(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int c;

    c = 9730;
    printf("in the default locale, iswgraph(%#x) = %d\n", c, !!iswgraph(c));
    setlocale(6, "en_US.utf8");
    printf("in Unicode locale, iswgraph(%#x) = %d\n", c, !!iswgraph(c));
}


