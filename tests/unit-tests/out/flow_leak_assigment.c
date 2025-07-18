struct X {
    char * name;
};


void *malloc(int i);

int main()
{
    struct X * p;

    p = malloc(4U);
    p = 0;
}

