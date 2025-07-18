struct X {
    char * text;
};


void *malloc(unsigned long size);
void x_delete(struct X * p);

int main()
{
    struct X * p;

    p = malloc(4U);
    x_delete(p);
}

