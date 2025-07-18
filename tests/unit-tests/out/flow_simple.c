struct X {
    char * name;
};


void *malloc(int i);
void free(void * p);

int main()
{
    struct X * p;

    p = malloc(4U);
    free(p);
}

