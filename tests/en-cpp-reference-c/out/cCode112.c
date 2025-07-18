struct _struct_tag_5 {
    int a[3];
    int b;
};


int a[2];
void *__cdecl malloc(unsigned int _Size);
void __cdecl free(void * _Block);

int main(void)
{
    int i;
    int k;
    int x[3];
    int * p;
    struct _struct_tag_5 w[2];
    char * ptr;

    k = 1;
    x[0] = 1;
    x[1] = 3;
    x[2] = 5;
    p = x;
    w.a[0] = 1;
    w.a[1] = 0;
    w.a[2] = 0;
    w.b = 0;
    w.a[0] = 2;
    w.a[1] = 0;
    w.a[2] = 0;
    w.b = 0;
    ptr = malloc(10);
    free(ptr);
}

