struct X {
    char * s;
};


char *strdup(char * s);

void init(struct X * px)
{
    px->s = strdup("a");
}

void free(void * p);

int main()
{
    struct X  x;

    init(&x);
    free(x.s);
}

