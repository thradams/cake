/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double strtod(char * __nptr, char ** __endptr);
extern int *__errno_location(void);

int main(void)
{
    char * p;
    char * end;

    p = "111.11 -2.22 Nan nan(2) inF 0X1.BC70A3D70A3D7P+6  1.18973e+4932zzz";
    printf("Parsing '%s':\n", p);
    {
        double f;

        f = strtod(p, &end);
        for (; p != end; f = strtod(p, &end))
        {
            printf("'%.*s' -> ", (int)(end - p), p);
            p = end;
            if ((*__errno_location()) == 34)
            {
                printf("range error, got ");
                (*__errno_location()) = 0;
            }
            printf("%f\n", f);
        }
    }
    printf("\"  -0.0000000123junk\"  -->  %g\n", strtod("  -0.0000000123junk", 0));
    printf("\"junk\"                 -->  %g\n", strtod("junk", 0));
}


