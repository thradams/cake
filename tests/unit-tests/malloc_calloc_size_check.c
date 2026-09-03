void* malloc(unsigned long size);
void* calloc(unsigned long nmemb, unsigned long size);

void f(void)
{
    int* p1 = malloc(1);                    //lint 89 warning: argument to 'malloc' is not a multiple of the pointee size (4)
    int* p2 = malloc(sizeof(int));
    int* p3 = malloc(4 * sizeof(*p3));
    int* p4 = malloc(4 * sizeof(int));

    int n = 3;
    int* p5 = malloc(n * sizeof *p5);

    float* p6 = malloc(4 * sizeof(int));
    void* p7 = malloc(1);

    int* p8;
    p8 = malloc(1);                         //lint 89 warning: argument to 'malloc' is not a multiple of the pointee size (4)
    p8 = malloc(sizeof(*p8));

    int* c1 = calloc(4, sizeof(int));
    int* c2 = calloc(4, sizeof(*c2));
    int* c3 = calloc(4, 1);                 //lint 89 warning: size argument to 'calloc' does not match the pointee size (4)
    int* c4 = calloc(4, 8);                 //lint 89 warning: size argument to 'calloc' does not match the pointee size (4)

    int* c5;
    c5 = calloc(4, sizeof(*c5));
    c5 = calloc(4, 1);                      //lint 89 warning: size argument to 'calloc' does not match the pointee size (4)
}
