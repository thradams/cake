/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswdigit(unsigned int __wc);
extern int iswctype(unsigned int __wc, unsigned int long __desc);
extern unsigned int long wctype(char * __property);

void test(int a3, int u3, int j3)
{
    printf("\t '%lc'  '%lc' '%lc'\n", a3, u3, j3);
    printf("iswdigit: %d    %d    %d\n", !!iswdigit(a3), !!iswdigit(u3), !!iswdigit(j3));
    printf("jdigit:   %d    %d    %d\n", !!iswctype(a3, wctype("jdigit")), !!iswctype(u3, wctype("jdigit")), !!iswctype(j3, wctype("jdigit")));
}


extern char *setlocale(int __category, char * __locale);
extern int puts(char * __s);

int main(void)
{
    int a3;
    int u3;
    int j3;

    a3 = 51;
    u3 = 19977;
    j3 = 65299;
    setlocale(6, "en_US.utf8");
    puts("In American locale:");
    test(a3, u3, j3);
    setlocale(6, "ja_JP.utf8");
    puts("\nIn Japanese locale:");
    test(a3, u3, j3);
}


