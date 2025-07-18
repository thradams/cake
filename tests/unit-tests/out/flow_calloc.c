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

    x = calloc(1, 16U);
    free(x);
}

