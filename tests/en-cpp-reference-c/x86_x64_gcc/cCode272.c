/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("signbit(+0.0) = %d\n", (0 ? __builtin_signbitf(+0.0) : 1 ? __builtin_signbit(0.0) : __builtin_signbitl(+0.0)));
    printf("signbit(-0.0) = %d\n", (0 ? __builtin_signbitf(-0.0) : 1 ? __builtin_signbit(-0.0) : __builtin_signbitl(-0.0)));
}


