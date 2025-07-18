struct X {
    char * p;
};


char *strdup(char * s);
void free(void * p);

int main()
{
    char * p0;
    struct X  x;

    p0 = strdup("a");
    x.p = p0;
    free(x.p);
}

