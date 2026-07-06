/* Cake x86_x64_gcc */
struct __tag17 {
    int a[3];
    int b;
};


int a[2];
extern void *malloc(unsigned int long __size);
extern void free(void * __ptr);

int main(void)
{
    int i;
    int k;
    int x[3];
    int * p;
    struct __tag17 w[2];
    char * ptr;

    k = 1;
    x[0] = 1;
    x[1] = 3;
    x[2] = 5;
    p = x;
    w[0].a[0] = 1;
    w[0].a[1] = 0;
    w[0].a[2] = 0;
    w[0].b = 0;
    w[1].a[0] = 2;
    w[1].a[1] = 0;
    w[1].a[2] = 0;
    w[1].b = 0;
    ptr = malloc(10);
    free(ptr);
}


