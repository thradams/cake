/* Cake x86_x64_gcc */

extern char *setlocale(int __category, char * __locale);
extern unsigned int long wcsxfrm(int * __s1, int * __s2, unsigned int long __n);
extern int printf(char * __format, ...);
extern int wcscmp(int * __s1, int * __s2);

int main(void)
{
    int * in1;
    int out1[];
    int * in2;
    int out2[];

    setlocale(6, "sv_SE.utf8");
    in1 = L"\u00e5r";
    wcsxfrm(out1, in1, 0 / 4L);
    in2 = L"\u00e4ngel";
    wcsxfrm(out2, in2, 0 / 4L);
    printf("In the Swedish locale: ");
    if (wcscmp(out1, out2) < 0)
    {
        printf("%ls before %ls\n", in1, in2);
    }
    else
    {
        printf("%ls before %ls\n", in2, in1);
    }
    printf("In lexicographical comparison: ");
    if (wcscmp(in1, in2) < 0)
    {
        printf("%ls before %ls\n", in1, in2);
    }
    else
    {
        printf("%ls before %ls\n", in2, in1);
    }
}


