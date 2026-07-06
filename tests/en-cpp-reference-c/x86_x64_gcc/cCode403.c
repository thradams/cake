/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double wcstod(int * __nptr, int ** __endptr);
extern int *__errno_location(void);

int main(void)
{
    int * p;
    int * end;

    p = L"111.11 -2.22 0X1.BC70A3D70A3D7P+6  1.18973e+4932zzz";
    printf("Parsing L\"%ls\":\n", p);
    {
        double f;

        f = wcstod(p, &end);
        for (; p != end; f = wcstod(p, &end))
        {
            printf("'%.*ls' -> ", (int)(end - p), p);
            p = end;
            if ((*__errno_location()) == 34)
            {
                printf("range error, got ");
                (*__errno_location()) = 0;
            }
            printf("%f\n", f);
        }
    }
}


