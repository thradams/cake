/* Cake 0.12.26 x86_msvc */
struct Y;

struct X {
    char * text;
    int * p1;
    int i;
    struct Y * pY;
};

struct Y {
    char * p0;
    int * p2;
    double i2;
};


void *calloc(unsigned long n, unsigned long size);
void free(void * ptr);

int main()
{
    struct X * x;

    x = calloc(1, 16);
    free(x);
}


