/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int iswxdigit(unsigned int __wc);
extern int iswctype(unsigned int __wc, unsigned int long __desc);
extern unsigned int long wctype(char * __property);

void test(int a3, int u3, int j3)
{
    printf("\t  '%lc'  '%lc' '%lc'\n", a3, u3, j3);
    printf("iswxdigit: %d    %d    %d\n", !!iswxdigit(a3), !!iswxdigit(u3), !!iswxdigit(j3));
    printf("jdigit:    %d    %d    %d\n", !!iswctype(a3, wctype("jdigit")), !!iswctype(u3, wctype("jdigit")), !!iswctype(j3, wctype("jdigit")));
}


extern char *setlocale(int __category, char * __locale);
extern int puts(char * __s);

int main(void)
{
    int a3;
    int u3;
    int j3;

    a3 = 57;
    u3 = 12329;
    j3 = 65305;
    setlocale(6, "en_US.utf8");
    puts("In American locale:");
    test(a3, u3, j3);
    setlocale(6, "ja_JP.utf8");
    puts("\nIn Japanese locale:");
    test(a3, u3, j3);
}


