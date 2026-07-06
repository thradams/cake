/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    int n;
    int a[];

    n = 1;
    label:
    ;
    printf("The array has %zu elements\n", 0 / 4L);
    if (n++ < 10)
    {
        goto label;
    }
}


