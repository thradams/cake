/* Cake x86_x64_gcc */

extern char *setlocale(int __category, char * __locale);
extern unsigned int long strxfrm(char * __dest, char * __src, unsigned int long __n);
extern int printf(char * __format, ...);
extern int strcmp(char * __s1, char * __s2);

int main(void)
{
    char * in1;
    char out1[];
    char * in2;
    char out2[];

    setlocale(3, "cs_CZ.iso88592");
    in1 = "hrnec";
    strxfrm(out1, in1, 0);
    in2 = "chrt";
    strxfrm(out2, in2, 0);
    printf("In the Czech locale: ");
    if (strcmp(out1, out2) < 0)
    {
        printf("%s before %s\n", in1, in2);
    }
    else
    {
        printf("%s before %s\n", in2, in1);
    }
    printf("In lexicographical comparison: ");
    if (strcmp(in1, in2) < 0)
    {
        printf("%s before %s\n", in1, in2);
    }
    else
    {
        printf("%s before %s\n", in2, in1);
    }
}


