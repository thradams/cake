/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern unsigned int long wcstoul(int * __nptr, int ** __endptr, int __base);
extern int *__errno_location(void);

int main(void)
{
    int * p;
    int * end;

    p = L"10 200000000000000000000000000000 30 40";
    printf("Parsing L'%ls':\n", p);
    {
        unsigned long i;

        i = wcstoul(p, &end, 10);
        for (; p != end; i = wcstoul(p, &end, 10))
        {
            printf("'%.*ls' -> ", (int)(end - p), p);
            p = end;
            if ((*__errno_location()) == 34)
            {
                printf("range error, got ");
                (*__errno_location()) = 0;
            }
            printf("%lu\n", i);
        }
    }
}


