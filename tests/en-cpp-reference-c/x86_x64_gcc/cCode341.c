/* Cake x86_x64_gcc */

extern char *strpbrk(char * __s, char * __accept);
extern unsigned int long strspn(char * __s, char * __accept);
extern int printf(char * __format, ...);

int main(void)
{
    char * str;
    char * sep;
    unsigned int cnt;

    str = "hello world, friend of mine!";
    sep = " ,!";
    cnt = 0;
    do
    {
        str = strpbrk(str, sep);
        if (str)
        {
            str += strspn(str, sep);
        }
        ++cnt;
    }
    while (str && *str);
    printf("There are %u words\n", cnt);
}


