/* Cake x86_x64_gcc */

int compare_ints(void * a, void * b)
{
    int arg1;
    int arg2;

    arg1 = *(int *)a;
    arg2 = *(int *)b;
    if (arg1 < arg2)
    {
        return -1;
    }
    if (arg1 > arg2)
    {
        return 1;
    }
    return 0;
}


extern void qsort(void * __base, unsigned int long __nmemb, unsigned int long __size, int (* __compar)(void *, void *));
extern int printf(char * __format, ...);

int main(void)
{
    int ints[7];
    int size;

    ints[0] = -2;
    ints[1] = 99;
    ints[2] = 0;
    ints[3] = -743;
    ints[4] = 2;
    ints[5] = -2147483648;
    ints[6] = 4;
    size = 7L;
    qsort(ints, size, 4L, compare_ints);
    {
        int i;

        i = 0;
        for (; i < size; i++)
        {
            printf("%d ", ints[i]);
        }
    }
    printf("\n");
}


