/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double atof(char * __nptr);

int main(void)
{
    printf("%g\n", atof("  -0.0000000123junk"));
    printf("%g\n", atof("0.012"));
    printf("%g\n", atof("15e16"));
    printf("%g\n", atof("-0x1afp-2"));
    printf("%g\n", atof("inF"));
    printf("%g\n", atof("Nan"));
    printf("%g\n", atof("1.0e+309"));
    printf("%g\n", atof("0.0"));
    printf("%g\n", atof("junk"));
}


