/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    unsigned int long N;
    int numbers[100];
    unsigned int long size;

    N = 100;
    {
        unsigned int long ndx;

        ndx = 0;
        for (; ndx < 100L; ++ndx)
        numbers[ndx] = ndx;
    }
    printf("SIZE_MAX = %zu\n", 18446744073709551615L);
    size = 400L;
    printf("size = %zu\n", size);
}


