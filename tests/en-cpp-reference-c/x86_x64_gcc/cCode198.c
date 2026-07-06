/* Cake x86_x64_gcc */

extern int fesetround(int __rounding_direction);
extern int puts(char * __s);
extern int printf(char * __format, ...);
extern float acosf(float __x);
extern float strtof(char * __nptr, char ** __endptr);
extern float rintf(float __x);

int main()
{
    fesetround(1024);
    puts("rounding down: ");
    printf("           pi = %.22f\n", acosf(-1));
    printf("strtof(\"1.1\") = %.22f\n", strtof("1.1", 0));
    printf("    rint(2.1) = %.22f\n\n", rintf(2.1000000000000001));
    fesetround(2048);
    puts("rounding up: ");
    printf("           pi = %.22f\n", acosf(-1));
    printf("strtof(\"1.1\") = %.22f\n", strtof("1.1", 0));
    printf("    rint(2.1) = %.22f\n", rintf(2.1000000000000001));
}


