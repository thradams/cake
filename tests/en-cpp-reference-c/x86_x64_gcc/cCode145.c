/* Cake x86_x64_gcc */

int A;
extern int printf(char * __format, ...);
extern void *malloc(unsigned int long __size);
extern void free(void * __ptr);

int main(void)
{
    int A;
    int * ptr_1;

    printf("&A = %p\n", (void *)&A);
    A = 1;
    printf("&A = %p\n", (void *)&A);
    ptr_1 = malloc(4L);
    printf("address of int in allocated memory = %p\n", (void *)ptr_1);
    free(ptr_1);
}


