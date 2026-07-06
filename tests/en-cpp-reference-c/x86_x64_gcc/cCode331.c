/* Cake x86_x64_gcc */

extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);
extern int strcoll(char * __s1, char * __s2);
extern int strcmp(char * __s1, char * __s2);

int main(void)
{
    char * s1;
    char * s2;

    setlocale(3, "cs_CZ.utf8");
    s1 = "hrnec";
    s2 = "chrt";
    printf("In the Czech locale: ");
    if (strcoll(s1, s2) < 0)
    {
        printf("%s before %s\n", s1, s2);
    }
    else
    {
        printf("%s before %s\n", s2, s1);
    }
    printf("In lexicographical comparison: ");
    if (strcmp(s1, s2) < 0)
    {
        printf("%s before %s\n", s1, s2);
    }
    else
    {
        printf("%s before %s\n", s2, s1);
    }
}


