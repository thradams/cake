/* Cake x86_x64_gcc */

extern int wcscoll(int * __s1, int * __s2);
extern int printf(char * __format, ...);

void try_compare(int * p1, int * p2)
{
    if (wcscoll(p1, p2) < 0)
    {
        printf("%ls before %ls\n", p1, p2);
    }
    else
    {
        printf("%ls before %ls\n", p2, p1);
    }
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    setlocale(6, "en_US.utf8");
    printf("In the American locale: ");
    try_compare(L"hrnec", L"chrt");
    setlocale(3, "cs_CZ.utf8");
    printf("In the Czech locale: ");
    try_compare(L"hrnec", L"chrt");
    setlocale(3, "en_US.utf8");
    printf("In the American locale: ");
    try_compare(L"\xc3\xa5r", L"\xc3\xa4ngel");
    setlocale(3, "sv_SE.utf8");
    printf("In the Swedish locale: ");
    try_compare(L"\xc3\xa5r", L"\xc3\xa4ngel");
}


