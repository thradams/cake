struct X {
    char * name;
};


void *malloc(unsigned long size);

int main()
{
    struct X * p;

    p = (struct X *)malloc(1);
}

