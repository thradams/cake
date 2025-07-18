struct Y {
    char * p0;
    int * p2;
    double i2;
};

struct X {
    char * text;
    int * p1;
    int i;
    struct Y * pY;
};


void *malloc(unsigned long size);
void free(void * ptr);

int main()
{
    struct X * x;

    x = malloc(16U);
    free(x);
}

