/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern unsigned int long strtoul(char * __nptr, char ** __endptr, int __base);
extern int *__errno_location(void);

int main(void)
{
    char * p;
    char * end;

    p = "10 200000000000000000000000000000 30 -40 - 42";
    printf("Parsing '%s':\n", p);
    end = 0;
    {
        unsigned long i;

        i = strtoul(p, &end, 10);
        for (; p != end; i = strtoul(p, &end, 10))
        {
            printf("'%.*s' -> ", (int)(end - p), p);
            p = end;
            if ((*__errno_location()) == 34)
            {
                (*__errno_location()) = 0;
                printf("range error, got ");
            }
            printf("%lu\n", i);
        }
    }
    printf("After the loop p points to '%s'\n", p);
}


