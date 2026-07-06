/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

void print_storage_info(int * next, int * prev, int ints)
{
    if (next)
    {
        printf("%s location: %p. Size: %d ints (%ld bytes).\n", (next != prev ? "New" : "Old"), (void *)next, ints, ints * 4L);
    }
    else
    {
        printf("Allocation failed.\n");
    }
}


extern void *malloc(unsigned int long __size);
extern void *memcpy(void * __dest, void * __src, unsigned int long __n);
extern void *realloc(void * __ptr, unsigned int long __size);
extern void free(void * __ptr);

int main(void)
{
    int pattern[8];
    int pattern_size;
    int * next;
    int * prev;
    int realloc_size[6];

    pattern[0] = 1;
    pattern[1] = 2;
    pattern[2] = 3;
    pattern[3] = 4;
    pattern[4] = 5;
    pattern[5] = 6;
    pattern[6] = 7;
    pattern[7] = 8;
    pattern_size = 8L;
    next = 0;
    prev = 0;
    if ((next = (int *)malloc(32L)))
    {
        memcpy(next, pattern, 32L);
        print_storage_info(next, prev, 8);
    }
    else
    {
        return 1;
    }
    realloc_size[0] = 10;
    realloc_size[1] = 12;
    realloc_size[2] = 512;
    realloc_size[3] = 32768;
    realloc_size[4] = 65536;
    realloc_size[5] = 32768;
    {
        int i;

        i = 0;
        for (; i != 6L; ++i)
        {
            if ((next = (int *)realloc(prev = next, realloc_size[i] * 4L)))
            {
                print_storage_info(next, prev, realloc_size[i]);
                ;
            }
            else
            {
                free(prev);
                return 1;
            }
        }
    }
    free(next);
    return 0;
}


